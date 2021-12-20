#include "../include/ghassanpl/errors.h"

//#include <format>
#include <gtest/gtest.h>

#include <boost/stacktrace.hpp>
#include <nlohmann/json.hpp>
using namespace std::string_view_literals;
using nlohmann::json;

namespace err = ghassanpl::err;
#if 0
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
#endif

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

namespace ghassanpl::err
{
	void to_json(json& j, decomposed_uri const& dec)
	{
		j["scheme"] = dec.scheme;
		j["authority"] = dec.authority;
		j["user_info"] = dec.user_info;
		j["host"] = dec.host;
		j["port"] = dec.port;
		j["path"] = dec.path;
		j["path_elements"] = dec.path_elements;
		j["normalized_path"] = dec.normalized_path();
		j["query"] = dec.query;
		j["query_elements"] = dec.query_elements;
		j["fragment"] = dec.fragment;
	}
}

void uri_equal(err::uri_view view, err::decomposed_uri&& uri, json&& whats)
{
	auto requested = (json)uri;
	for (auto& kvp : requested.get<json::object_t>())
	{
		if (whats.contains(kvp.first))
			EXPECT_EQ(whats[kvp.first], kvp.second) << "when comparing field " << kvp.first << " of uri " << view;
		//else
			//EXPECT_EQ(std::string{ "" }, kvp.second) << "when comparing field " << kvp.first << " of uri " << view;
	}
}

TEST(addresses, uri_decompose_works)
{
#define URI(s, ...) uri_equal(s, err::decompose_uri(s).value(), json::object_t(__VA_ARGS__));
#include "uri_tests.inc"
#undef URI
#define URI(s, ...) std::cout << std::format("==> {}\n{}\n", s, json(err::decompose_uri(s).value()).dump(2));
#include "uri_tests.inc"
#undef URI
}

TEST(addresses, uri_decompose_properly_catches_degenerate_cases)
{
	auto uri = err::decompose_uri("FTP://cnn.example.com&story=breaking_news@10.0.0.1/top_story.htm").value();
	EXPECT_EQ(uri.host, "10.0.0.1");
	EXPECT_EQ(uri.path, "/top_story.htm");
	EXPECT_EQ(uri.user_info, "cnn.example.com&story=breaking_news");
	EXPECT_EQ(uri.scheme, "ftp");
}

TEST(logging, basic_logging_works)
{
	err::level::error("something broke: {}", 7);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
