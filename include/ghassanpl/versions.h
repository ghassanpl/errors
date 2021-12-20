#pragma once

#include "detail/common.h"

namespace ghassanpl::err
{
	template <typename>
	inline std::string_view compilation_date() noexcept { return __DATE__ "-" __TIME__; }

}