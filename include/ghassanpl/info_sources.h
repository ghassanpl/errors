#pragma once

#include "detail/common.h"

#include <span>
#include <set>
#include <string>

#include "X:\Code\Native\ghassanpl\header_utils\include\ghassanpl\with_sl.h"

namespace ghassanpl::err
{
	enum class changes_per
	{
		compilation,
		machine,
		execution,
		log_domain,
		thread,
		call_site,
		query,
	};

	struct info_datum
	{
		std::span<char const> data() const noexcept { return mData; }
		info_source const& parent_info_source() const noexcept { return *mParentInfoSource; };

		info_datum(info_datum const&) noexcept = default;
		info_datum(info_datum&&) noexcept = default;
		info_datum& operator=(info_datum const&) noexcept = default;
		info_datum& operator=(info_datum&&) noexcept = default;

		bool operator==(info_datum const& other) const noexcept = default;
		auto operator<=>(info_datum const& other) const noexcept = default;

	private:

		friend struct info_source;

		info_datum(info_source const* parent, std::string data) : mParentInfoSource(parent), mData(std::move(data)) {}

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

		virtual bool is_mutable() const noexcept = 0;
		virtual info_datum querry_current() noexcept = 0;
		virtual std::string_view name() const noexcept = 0;
		virtual std::string to_string(info_datum const& datum) const = 0;
		virtual info_datum deserialize_from_buffer(std::span<char const> buffer) = 0;

		virtual std::string querry_current_as_string() { return to_string(querry_current()); }

		bool operator==(info_source const& other) const noexcept { return this == std::addressof(other); }
		auto operator<=>(info_source const& other) const noexcept { return this <=> std::addressof(other); }

		static auto& active_info_sources() noexcept { return mActiveInfoSources; }

	protected:

		info_datum make_info_datum(std::string data) const noexcept { return { this, std::move(data) }; }

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

	struct simple_string_info_source : info_source
	{
		simple_string_info_source(std::string name, bool is_mutable) : mName(std::move(name)), mMutable(is_mutable) {}

		virtual bool is_mutable() const noexcept override { return mMutable; }
		virtual info_datum querry_current() noexcept override { return make_info_datum(get_string_datum()); }
		virtual std::string querry_current_as_string() override { return get_string_datum(); }
		virtual std::string_view name() const noexcept override { return mName; }
		virtual std::string to_string(info_datum const& datum) const override { return std::string{ datum.data() }; }
		virtual info_datum deserialize_from_buffer(std::span<char const> buffer) override { return make_info_datum(std::string{ buffer.begin(), buffer.end() }); }

	protected:

		std::string mName;
		bool mMutable = false;
		virtual std::string get_string_datum() noexcept = 0;

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
	///		- other network/connectivity topology information
	///		- cpuid
	///		- user
	///		- environment variables
	///		- OS name and info
	///		- memory size and stats https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/ns-sysinfoapi-memorystatusex
	///		- disks and other NV-storage
	/// 
	/// - app id/guid/name
	
	namespace info_sources
	{
		/// Immutable information set at compile time

		info_source& build_version();
		info_source& git_commit_hash();

		info_source& compilation_date();
		info_source& compiler_version();
		info_source& compilation_flags(); /// NDEBUG/_DEBUG, runtime checks, extensions, etc.
		info_source& extensions_list(); /// openmp, intel tbb, asan, etc.

		info_source& language_version();

		info_source& target_isa();
		info_source& used_isa_extensions();
		
		info_source& char_type_properties(); /// char signedness, wchar_t size, etc
		info_source& integral_type_properties(); /// sizeofs for all standard integral types and typedefs, including bool
		info_source& floating_point_type_properties(); /// sizeofs and formats of all floating point types, also gives the bit pattern of +0, -0, 1, -1, -inf, +inf and NaN for each
		info_source& floating_point_math(); /// Whether precise or fast fp math instructions are used
		info_source& time_properties(); /// timestamp/clock resolutions, offsets, sources, etc https://datatracker.ietf.org/doc/html/rfc5424#section-7.1.3

		/// Default info-sources per message

		info_source& log_point();
		info_source& timestamp();
		info_source& message_string();
	}

}
