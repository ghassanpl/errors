#pragma once

#if 0
#include "detail/common.h"

#include <system_error>

namespace ghassanpl::err
{
	template <typename ERROR_TYPE = std::error_code>
	struct basic_error_result;

	namespace detail
	{
		template <typename ERROR_TYPE, typename EXCEPTION_TYPE>
		concept error_code_accepting_exception = std::constructible_from<EXCEPTION_TYPE, ERROR_TYPE, std::source_location> || std::constructible_from<EXCEPTION_TYPE, ERROR_TYPE>;
	}

	template <typename ERROR_TYPE, typename EXCEPTION_TYPE>
	requires detail::error_code_accepting_exception<ERROR_TYPE, EXCEPTION_TYPE>
	struct throws_type
	{
		using exception_type = EXCEPTION_TYPE;
	};

	static constexpr inline throws_type<std::error_code, std::system_error> throws;

	template <typename EXCEPTION_TYPE>
	static constexpr inline auto throws_exception() {
		static_assert(detail::error_code_accepting_exception<std::error_code, EXCEPTION_TYPE>, "exception type must be constructible from either an std::error_code or a an std::error_code an and std::source_location");
		return throws_type<std::error_code, EXCEPTION_TYPE>{};
	}

	template <typename ERROR_CODE, typename EXCEPTION_TYPE>
	static constexpr inline auto throws_exception() {
		static_assert(detail::error_code_accepting_exception<ERROR_CODE, EXCEPTION_TYPE>, "exception type must be constructible from either an std::error_code or a an std::error_code an and std::source_location");
		return throws_type<ERROR_CODE, EXCEPTION_TYPE>{};
	}

	template <typename ERROR_TYPE>
	struct basic_error_result
	{
		using error_type = ERROR_TYPE;

		using function_on_error = void(*)(basic_error_result const&, error_type const&);

#if !defined(_MSC_VER) || _MSC_VER > 1931

		/// Will reset 'result' to {}, and will assign the error code given to 'result'.
		basic_error_result(error_type& result, std::source_location location = std::source_location::current())
			: mTarget(&result), mSourceLocation(location)
		{
			result = {};
		}

		/// will call the given function when assigned an error
		basic_error_result(function_on_error func, std::source_location location = std::source_location::current())
			: mFunctionOnError(func), mSourceLocation(location)
		{
		}

		/// will call the given function when assigned an error
		template <typename EXCEPTION_TYPE>
		basic_error_result(throws_type<ERROR_TYPE, EXCEPTION_TYPE>, std::source_location location = std::source_location::current())
			: mFunctionOnError(&throw_exception<EXCEPTION_TYPE>), mSourceLocation(location)
		{
		}

#else

		/// Will reset 'result' to {}, and will assign the error code given to 'result'.
		basic_error_result(error_type& result)
			: mTarget(&result)
		{
			result = {};
		}

		/// will call the given function when assigned an error
		basic_error_result(function_on_error func)
			: mFunctionOnError(func)
		{
		}

		/// will call the given function when assigned an error
		template <typename EXCEPTION_TYPE>
		requires detail::error_code_accepting_exception<error_type, EXCEPTION_TYPE>
		basic_error_result(throws_type<error_type, EXCEPTION_TYPE>)
			: mFunctionOnError(&throw_exception<EXCEPTION_TYPE>)
		{
		}

#endif

		~basic_error_result() noexcept = default;

		template <typename ET = error_type>
		requires std::assignable_from<error_type&, ET&&>
		basic_error_result& operator=(ET&& ec)
		{
			if (mTarget)
				*mTarget = std::forward<ET>(ec);
			else if (ec && mFunctionOnError)
				mFunctionOnError(*this, std::forward<ET>(ec));
			return *this;
		}

		/// TODO: only enable this function if error_type is copyable
		[[nodiscard]] explicit operator error_type() const noexcept requires std::copy_constructible<error_type> { if (mTarget) return *mTarget; return {}; }

		[[nodiscard]] explicit operator bool() const noexcept requires std::convertible_to<error_type, bool> { return (bool)operator error_type(); }

		[[nodiscard]] auto const& source_location() const noexcept { return mSourceLocation; }

	private:

		template <typename T>
		static void throw_exception(basic_error_result const& er, error_type const& ec)
		{
			if constexpr (std::constructible_from<T, error_type, std::source_location>)
				throw T{ ec, er.source_location() };
			else if constexpr (std::constructible_from<T, error_type>)
				throw T{ ec };
		}

		error_type* mTarget = nullptr;
		function_on_error mFunctionOnError = nullptr;
		std::source_location mSourceLocation;
	};

	using error_result = basic_error_result<>;

}

#endif