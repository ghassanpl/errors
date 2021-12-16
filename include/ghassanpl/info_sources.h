#pragma once

#include "detail/common.h"

#include <span>
#include <set>
#include <string>

#include "X:\Code\Native\ghassanpl\header_utils\include\ghassanpl\with_sl.h"

namespace ghassanpl::err
{
	struct info_source;

	struct info_datum
	{
		std::span<char const> data() const noexcept { return mData; }
		info_source const& parent_info_source() const noexcept { return *mParentInfoSource; };

	private:

		friend struct info_source;

		info_source const* mParentInfoSource{};
		std::string mData;
	};

	struct info_source
	{
		info_source();
		virtual ~info_source() noexcept;

		info_source(info_source const&) = delete;
		info_source(info_source&&) = delete;
		info_source& operator=(info_source const&) = delete;
		info_source& operator=(info_source&&) = delete;

		virtual info_datum querry_current() noexcept = 0;
		virtual std::string_view name() const noexcept = 0;
		virtual std::string to_string(info_datum const& datum) const = 0;

		bool operator==(info_source const& other) const noexcept { return this == std::addressof(other); }
		auto operator<=>(info_source const& other) const noexcept { return this <=> std::addressof(other); }

		static auto& active_info_sources() noexcept { return mActiveInfoSources; }

	private:

		thread_local static std::set<info_source*> mActiveInfoSources;
	};

	struct function_call_info_source : info_source
	{
		template <typename... ARGS>
		function_call_info_source(with_sl<std::string_view> func_name, ARGS const&... arguments)
		{

		}
	};


	/// - errno
	/// - GetLastError
	/// 
	/// - OpenGL
	/// 
	/// - floating point/simd errors and env
	/// - process/thread info
	/// - system info:
	///		- mac address
	///		- hostname
	///		- cpuid
	///		- user

}
