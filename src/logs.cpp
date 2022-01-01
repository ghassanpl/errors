#include "../include/ghassanpl/logs.h"

#include <mutex>
#include <atomic>
#include <map>
#include <filesystem>

namespace ghassanpl::err
{
	namespace level
	{
		log_entry_type trace{"trace"};
		log_entry_type debug{"debug"};
		log_entry_type info{"info"};
		log_entry_type warning{"warning"};
		log_entry_type error{"error"};
		log_entry_type fatal{"fatal"};
	}

	namespace
	{
		using source_location_hash = size_t;

		constexpr auto source_location_hasher = [](std::source_location const& sl) { return source_location_hash{}; };

		struct log_static
		{
			std::mutex mLogConfigMutex;
			std::set<log_domain const*> mRegisteredDomains;
			std::map<std::filesystem::path, log_domain const*, std::less<>> mPerFileDomains;
			log_domain const default_application_domain{ "application" };
			std::set<log_entry_type const*> mRegisteredEntryTypes;

			std::unordered_map<source_location_hash, log_point> mAutomaticLogPoints;

			std::atomic<size_t> mLogPointIDs = 0;

			size_t new_lp_id()
			{
				const auto result = ++mLogPointIDs;
				/// AssumingNotEqual(result, 0);
				return result;
			}

			void register_log_point(log_point const&)
			{
				std::lock_guard guard{ mLogConfigMutex };

				throw "unimplemented";
			}

			log_static()
			{
				register_domain(default_application_domain);

				register_entry_type(level::trace);
				register_entry_type(level::debug);
				register_entry_type(level::info);
				register_entry_type(level::warning);
				register_entry_type(level::error);
				register_entry_type(level::fatal);
			}

			static log_static& get() { static log_static ls; return ls; }
		};
	}

	void register_domain(log_domain const& domain)
	{
		auto& ls = log_static::get();

		std::lock_guard guard{ ls.mLogConfigMutex };
		ls.mRegisteredDomains.insert(&domain);
	}

	void unregister_domain(log_domain const& domain)
	{
		auto& ls = log_static::get();

		std::lock_guard guard{ ls.mLogConfigMutex };
		ls.mRegisteredDomains.erase(&domain);
	}

	std::set<log_domain const*> registered_domains()
	{
		auto& ls = log_static::get();

		std::lock_guard guard{ ls.mLogConfigMutex };
		return ls.mRegisteredDomains;
	}

	log_domain const& default_application_log_domain()
	{
		return log_static::get().default_application_domain;
	}

	void register_entry_type(log_entry_type const& type)
	{
		auto& ls = log_static::get();

		std::lock_guard guard{ ls.mLogConfigMutex };
		ls.mRegisteredEntryTypes.insert(&type);
	}
	
	void unregister_entry_type(log_entry_type const& type)
	{
		auto& ls = log_static::get();

		std::lock_guard guard{ ls.mLogConfigMutex };
		ls.mRegisteredEntryTypes.erase(&type);
	}

	std::set<log_entry_type const*> registered_entry_types()
	{
		auto& ls = log_static::get();

		std::lock_guard guard{ ls.mLogConfigMutex };
		return ls.mRegisteredEntryTypes;
	}

	log_return_type sink_log_event(log_event event)
	{
		throw "unimplemented";
	}

	void add_datum(info_source const& src);

	log_return_type sink_log_entry(log_point const& lp, std::string_view message)
	{
		std::string data;
		log_event event{ log_event_type::log_entry, &lp, data };
		
		throw "unimplemented";

		return sink_log_event(std::move(event));
	}

	log_point const& detail::get_log_point(log_entry_type const& type, log_domain const& domain, std::source_location const& location)
	{
		auto& ls = log_static::get();

		auto loc_hash = source_location_hasher(location);

		std::lock_guard guard{ ls.mLogConfigMutex };
		auto it = ls.mAutomaticLogPoints.find(loc_hash);

		if (it == ls.mAutomaticLogPoints.end())
			return ls.mAutomaticLogPoints.emplace(loc_hash, log_point{type, domain, location}).first->second;
		return it->second;
	}

	log_domain const& detail::get_log_domain(std::source_location const& location)
	{
		auto& ls = log_static::get();

		std::lock_guard guard{ ls.mLogConfigMutex };
		if (auto it = ls.mPerFileDomains.find(std::filesystem::weakly_canonical(location.file_name())); it != ls.mPerFileDomains.end())
			return *it->second;
		return ls.default_application_domain;
	}

	namespace
	{
		log_domain const& get_per_file_domain(log_domain const& domain, std::filesystem::path const& path)
		{
			auto& ls = log_static::get();

			std::lock_guard guard{ ls.mLogConfigMutex };
			if (auto it = ls.mPerFileDomains.find(path); it != ls.mPerFileDomains.end())
			{
				if (&domain != it->second)
				{
					/// NOTE: We can't use any fields of the 'domain' object here, because it may have not been constructed yet
					/// as this function is called by domain_for_file constructors
					throw std::logic_error(std::format("a domain was already declared for file '{}'", path.string()));
				}
			}
			else
			{
				ls.mPerFileDomains[path] = &domain;
			}

			return domain;
		}
	}
	
	domain_for_file::domain_for_file(log_domain const& domain, std::source_location loc)
		: mDomain(get_per_file_domain(domain, std::filesystem::weakly_canonical(loc.file_name())))
	{
	}

	log_point::log_point(log_entry_type const& type_, log_domain const& domain_, std::source_location location_)
		: type(type_), domain(domain_), location(location_), id(log_static::get().new_lp_id())
	{
		auto& ls = log_static::get();
		ls.register_log_point(*this);
	}

}