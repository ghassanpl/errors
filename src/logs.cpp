#include "../include/ghassanpl/logs.h"

#include <mutex>
#include <atomic>
#include <map>
#include <filesystem>

namespace ghassanpl::err
{
	namespace
	{
		struct source_location_hash {};

		constexpr auto source_location_hasher = [](std::source_location const& sl) { return source_location_hash{}; };

		struct log_static
		{
			std::mutex mLogConfigMutex;
			std::set<log_domain const*> mRegisteredDomains;
			std::map<std::filesystem::path, log_domain const*, std::less<>> mPerFileDomains;
			log_domain const default_application_domain{ "application" };
			std::set<log_entry_type const*> mRegisteredEntryTypes;

			std::unordered_map<source_location_hash, log_point const*, decltype(source_location_hasher)> mSourceLocations;

			static log_static& get() { static log_static ls; return ls; }
		};
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

	struct log_event_builder
	{
		void add_datum(info_source const& src);

		log_event get_event();
	};

	log_return_type sink_log_entry(log_point const& lp, std::string_view message)
	{
		log_event_builder builder;
		throw "unimplemented";
		return sink_log_event(builder.get_event());
	}

	log_point const& detail::get_log_point(log_entry_type const& entry, log_domain const& domain, std::source_location const& location)
	{
		throw "unimplemented";
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
					/// as this function is called by per_file_domain constructors
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
	
	per_file_domain::per_file_domain(log_domain const& domain, std::source_location loc)
		: mDomain(get_per_file_domain(domain, std::filesystem::weakly_canonical(loc.file_name())))
	{
	}

	log_entry_type::log_entry_type(std::string_view _name)
		: name(_name)
	{
		auto& ls = log_static::get();

		std::lock_guard guard{ ls.mLogConfigMutex };
		ls.mRegisteredEntryTypes.insert(this);
	}

}