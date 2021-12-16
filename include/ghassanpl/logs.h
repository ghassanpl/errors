#pragma once

#include "detail/common.h"

#include "info_sources.h"

#include <vector>

namespace ghassanpl::err
{

	struct log_partition
	{
		std::string name;
		std::vector<info_source*> info_sources_to_query;
	};

	enum class log_response
	{
		none,
		report,
		abort,
	};

	struct log_entry_type
	{
		std::string name;
		std::vector<info_source*> info_sources_to_query;

		log_response default_response = log_response::none;

		int priority = 0;
		uint64_t filter_bitmask = 0x0;
	};

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
		log_partition const& partition;
		log_entry_type const& type;
		std::source_location location{};
	};

}