#pragma once

#include <string>
#include <string_view>

#include <ghassanpl/enum_flags.h>

namespace ghassanpl::err
{
	template <typename CHAR>
	using basic_uri = std::basic_string<CHAR>;
	
	template <typename CHAR>
	using basic_uri_view = std::basic_string_view<CHAR>;

	using uri = basic_uri<char>;
	using uri_view = basic_uri_view<char>;
}
