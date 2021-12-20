#pragma once

#include "detail/common.h"

#include "info_sources.h"

#include <vector>
#include <format>

namespace ghassanpl::err
{
	using log_return_type = void;
	
	/// self-registering class
	struct log_domain
	{
		log_domain(std::string_view name);
		~log_domain() noexcept;

		std::string name;
		std::vector<info_source*> info_sources_to_query;
	};

	template <typename T>
	concept log_domain_ref = std::convertible_to<T, log_domain const&> || std::constructible_from<std::string_view, T&&>;

	std::set<log_domain const*> registered_domains();

	log_domain const& default_application_log_domain();

	struct per_file_domain
	{
		per_file_domain(log_domain const& domain, std::source_location loc = std::source_location::current());
		
		/// TODO: per_file_domain(log_domain_ref auto domain, std::source_location loc = std::source_location::current());
		/// This would store the name of the domain if given, and only resolve to the actual domain when asked for

		log_domain const& domain() const noexcept { return mDomain; }

	private:
		
		log_domain const& mDomain;
	};

	enum class log_response
	{
		none,
		report,
		abort,
	};

	struct log_entry_type
	{
		log_entry_type(std::string_view name);
		~log_entry_type() noexcept;

		std::string name;
		std::vector<info_source*> info_sources_to_query;

		log_response default_response = log_response::none;

		int priority = 0;
		uint64_t filter_bitmask = 0x0;

		template <typename... ARGS>
		log_return_type operator()(with_sl<std::string_view> fmt, ARGS&&... args) const;
	};

	std::set<log_entry_type const*> registered_entry_types();

	namespace level
	{
		extern log_entry_type trace;
		extern log_entry_type debug;
		extern log_entry_type info;
		extern log_entry_type warning;
		extern log_entry_type error;
		extern log_entry_type fatal;
	}

	struct log_point
	{
		log_point(log_entry_type const& type_, log_domain const& domain_ = default_application_log_domain(), std::source_location location_ = std::source_location::current())
			: type(type_), domain(domain_), location(location_)
		{
		}

		log_entry_type const& type;
		log_domain const& domain;
		std::source_location const location{};

		constexpr bool operator==(log_point const& other) const noexcept { return this == std::addressof(other); }
		constexpr auto operator<=>(log_point const& other) const noexcept { return this <=> std::addressof(other); }
	};

	enum class log_event_type
	{
		log_entry,

		registered_domain,
		registered_entry_type,
		registered_log_point,

		thread_started,
		thread_stopped,

		logger_sync,
		logger_command,

		system_time_sync,
		system_suspend,
		system_wake_up,

		sink_change,
		info_source_update,
	};

	struct log_event {
		log_event_type type{};
		union {
			log_point const* point{};
			log_domain const* domain;
			log_entry_type const* entry_type;
			uint64_t integral_or_pointer_data;
		};
		std::string_view data;
	};

	log_return_type sink_log_entry(log_point const& lp, std::string_view message);
	log_return_type sink_log_event(log_event event);

	namespace detail
	{
		log_point const& get_log_point(log_entry_type const& entry, log_domain const& domain, std::source_location const& location);
		log_domain const& get_log_domain(std::source_location const& location);
		log_domain const& get_log_domain(std::string_view name);
		inline log_domain const& get_log_domain(log_domain const& domain) { return domain; }
		log_entry_type const& get_log_entry_type(std::string_view name);
		inline log_entry_type const& get_log_entry_type(log_entry_type const& type) { return type; }
	}

	template <typename... ARGS>
	requires (sizeof...(ARGS) > 0)
	log_return_type log(log_point const& lp, std::string_view fmt, ARGS&&... args)
	{
		return sink_log_entry(lp, std::format(fmt, std::forward<ARGS>(args)...));
	}

	inline log_return_type log(log_point const& lp, std::string_view fmt)
	{
		return sink_log_entry(lp, fmt);
	}

	template <typename T>
	concept log_entry_type_ref = std::convertible_to<T, log_entry_type const&> || std::constructible_from<std::string_view, T&&>;

	template <typename... ARGS>
	log_return_type log(log_entry_type_ref auto entry, log_domain_ref auto domain, std::source_location location, std::string_view fmt, ARGS&&... args)
	{
		auto const& lp = detail::get_log_point(detail::get_log_entry_type(entry), detail::get_log_domain(domain), location);
		log(lp, fmt, std::forward<ARGS>(args)...);
	}

	template <typename... ARGS>
	log_return_type log(log_entry_type_ref auto entry, std::source_location location, std::string_view fmt, ARGS&&... args)
	{
		auto const& ld = detail::get_log_domain(location);
		auto const& lp = detail::get_log_point(detail::get_log_entry_type(entry), ld, location);
		log(lp, fmt, std::forward<ARGS>(args)...);
	}

	template <typename... ARGS>
	log_return_type log(log_entry_type_ref auto entry, log_domain_ref auto domain, with_sl<std::string_view> fmt, ARGS&&... args)
	{
		auto const& lp = detail::get_log_point(detail::get_log_entry_type(entry), detail::get_log_domain(domain), fmt.Location);
		log(lp, fmt.Object, std::forward<ARGS>(args)...);
	}

	template <typename... ARGS>
	log_return_type log(log_entry_type_ref auto entry, with_sl<std::string_view> fmt, ARGS&&... args)
	{
		auto const& ld = detail::get_log_domain(fmt.Location);
		auto const& lp = detail::get_log_point(detail::get_log_entry_type(entry), ld, fmt.Location);
		log(lp, fmt.Object, std::forward<ARGS>(args)...);
	}

	template <typename... ARGS>
	log_return_type log_entry_type::operator()(with_sl<std::string_view> fmt, ARGS&&... args) const
	{
		return ghassanpl::err::log(*this, fmt.Location, fmt.Object, std::forward<ARGS>(args)...);
	}

}