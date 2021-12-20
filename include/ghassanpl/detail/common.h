#pragma once

#include <string>
#include <string_view>
#include <source_location>

#include <ghassanpl/enum_flags.h>

#include <tl/expected.hpp>

namespace ghassanpl::err
{
	using tl::expected;
	using tl::unexpected;

	/// URIs are stored in a UTF-8 encoding where both non-ASCII code unit bytes as well as URI-reserved characters (delimiters, etc) are %-encoded
	using uri = std::string;
	using uri_view = std::string_view;

	struct info_source;
}

