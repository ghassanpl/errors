#include "../include/ghassanpl/errors.h"

//#include <format>
#include <gtest/gtest.h>

#include <boost/stacktrace.hpp>

namespace err = ghassanpl::err;

void function(std::error_code error_to_report, err::error_result result = err::throws)
{
	result = error_to_report;
}

struct custom_exception : std::system_error
{
	std::source_location location;
	custom_exception(std::error_code ec, std::source_location sl)
		: system_error(ec), location(sl)
	{
;
	}
};

TEST(results, throwing_error_result_works)
{
	EXPECT_THROW(function(std::make_error_code(std::errc::invalid_argument)), std::system_error);
	EXPECT_THROW(function(std::make_error_code(std::errc::invalid_argument), err::throws), std::system_error);
	EXPECT_THROW(function(std::make_error_code(std::errc::invalid_argument), err::throws_exception<custom_exception>()), custom_exception);

	try
	{
		function(std::make_error_code(std::errc::invalid_argument), err::throws_exception<custom_exception>());
	}
	catch (custom_exception const& e)
	{
		EXPECT_STREQ(e.location.file_name(), std::source_location::current().file_name());
	}

	std::error_code ec;
	EXPECT_NO_THROW(function(std::make_error_code(std::errc::invalid_argument), ec));
	EXPECT_EQ(ec, std::make_error_code(std::errc::invalid_argument));

#if defined(_MSC_VER)
	static constexpr auto visual_studio_version = _MSC_VER;
	EXPECT_GT(visual_studio_version, 1931) << "Warning: MSVC 1931 has a bug wherein default arguments to functions cannot contain source_location constructors";
#endif
}

TEST(exceptions, for_each_nested_exception_works)
{
}

void err::uncaught_exception_callback()
{
	try
	{
		std::rethrow_exception(std::current_exception());
	}
	catch (std::runtime_error const& e)
	{
		EXPECT_STREQ(e.what(), "wut");
	}
}

TEST(exceptions, catch_the_uncatchable_works)
{
	err::catch_the_uncatchable([] {
		[] { [] { throw std::runtime_error("wut"); }(); } ();
	});
}

TEST(addresses, uri_decompose_works)
{
	//auto uri = err::decompose_uri("https://www.lol.com:244/asd/p?a=size#heh");
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
