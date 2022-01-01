#pragma once

#include "detail/common.h"
#include <exception>
#include <csetjmp>

namespace ghassanpl::err
{
	/// TODO: Ways to interact with system exceptions (win seh, arm faults, etc)
	/// https://wiki.segger.com/Cortex-M_Fault
	/// https://docs.microsoft.com/en-us/windows/win32/debug/structured-exception-handling
	/// https://docs.microsoft.com/en-us/cpp/cpp/structured-exception-handling-c-cpp?view=msvc-170

	/// <summary>
	///  
	/// </summary>
	/// <typeparam name="FUNC"></typeparam>
	/// <param name="e"></param>
	/// <param name="func"></param>

	template <typename FUNC>
	void for_each_nested_exception(std::exception const& e, FUNC&& func)
	{
		func(e);
		try
		{
			std::rethrow_if_nested(e);
		}
		catch (const std::exception& e)
		{
			for_each_nested_exception(e, func);
		}
		catch (...)
		{
			func(std::current_exception());
		}
	}

	void uncaught_exception_callback();

	/// Executes 'func'. If an uncaught exception is thrown while executing func, uncaught_exception_callback() will be called.
	/// 
	/// uncaught_exception_callback() will hopefully (depending on the compiler/library implementation) be called
	/// AT THE POINT OF THE THROW, allowing the programmer to capture a stack trace and some additional
	/// information before std::terminate() is called.
	/// 
	/// You need to provide the implementation of uncaught_exception_callback() yourself.
	/// 
	/// NOTE: Code inside 'func' will be executed as-if the std::terminate() was called at the point of the uncaught exception,
	/// so the stack will most likely not be unwound, destructors for the stack frames not called, etc. Only call this function
	/// if you plan to cleanup and terminate the program afterwards.
	/// 
	/// A good idea for usage is to request an interactive debugger hookup in uncaught_exception_callback(), or just dump the stack trace
	/// information into the logs/disk, and terminate.
	template <typename FUNC>
	void catch_the_uncatchable(FUNC&& func)
	{
		static thread_local std::jmp_buf buf;
		//static thread_local std::jmp_buf buf;

		const auto old_handler = std::set_terminate([] {
			uncaught_exception_callback();
			longjmp(buf, 1);
		});

		if (setjmp(buf) == 0)
		{
			[func = std::forward<FUNC>(func)] () noexcept {
				func();
			}();
		}

		std::set_terminate(old_handler);
	}
}