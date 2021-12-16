#pragma once

#include "common.h"

#include "../results.h"

#include <vector>
#include <stdexcept>
#include <system_error>

namespace ghassanpl::err
{
	/// https://github.com/austinsc/Poco/blob/master/Foundation/include/Poco/URI.h
	/// https://docs.pocoproject.org/current/Poco.URI.html
	/// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3420.html
	
	struct uri_parsing_error : std::runtime_error
	{

	};

	struct uri_path_parsing_error : uri_parsing_error
	{

	};

	std::error_category const& uri_category();

	static constexpr inline throws_type<std::error_code, uri_parsing_error> throws_uri_parsing_error;

	template <typename ELEMENT_TYPE>
	struct decomposed_uri
	{
		using element_type = ELEMENT_TYPE;

		element_type scheme{};
		element_type authority{};
			element_type user_info{};
			element_type host{};
			unsigned short port{};
		element_type path{};
			std::vector<element_type> path_elements;
		element_type query{};
			std::vector<std::pair<element_type, element_type>> query_elements;
		element_type fragment{};

		bool empty() const noexcept { return scheme.empty(); }
		bool valid() const noexcept {}

		uri compose() const {}
	};

	enum class uri_decompose_flags
	{
		split_authority_elements,
		split_path_elements,
		split_query_elements,
		use_well_known_port_numbers, ///< if a port is not specified in the uri, the result will guess the port based on the scheme

		convert_plus_to_space, ///< only valid for decode_uri
	};

	template <typename CHAR_TYPE>
	decomposed_uri<std::basic_string_view<CHAR_TYPE>> decompose_uri(basic_uri_view<CHAR_TYPE> uri, enum_flags<uri_decompose_flags> = enum_flags<uri_decompose_flags>::all(), error_result result = throws_uri_parsing_error)
	{
	}

	template <typename CHAR_TYPE>
	decomposed_uri<std::basic_string<CHAR_TYPE>> decode_uri(basic_uri_view<CHAR_TYPE> uri, enum_flags<uri_decompose_flags> = enum_flags<uri_decompose_flags>::all(), error_result result = throws_uri_parsing_error)
	{
	}

	enum class uri_compose_flags
	{
		path_leading_slash,
		path_trailing_slash,
		normalize_path,
	};

	template <typename ELEMENT_TYPE>
	basic_uri<typename ELEMENT_TYPE::value_type> compose_uri(decomposed_uri<ELEMENT_TYPE> const& decomposed, enum_flags<uri_decompose_flags> = enum_flags<uri_decompose_flags>::all(), error_result result = throws_uri_parsing_error)
	{

	}

	struct known_uri_scheme
	{
		virtual ~known_uri_scheme() noexcept = default;

		virtual std::error_code validate(uri_view uri) const noexcept = 0;
		virtual std::string_view scheme() const noexcept = 0;

		virtual std::error_code validate_authority(std::string_view uri) const noexcept = 0;
		virtual std::error_code validate_user_info(std::string_view uri) const noexcept = 0;
		virtual std::error_code validate_host(std::string_view uri) const noexcept = 0;
		virtual std::error_code validate_port(uint16_t port) const noexcept = 0;
		virtual std::error_code validate_path(std::string_view uri) const noexcept = 0;
		virtual std::error_code validate_query(std::string_view uri) const noexcept = 0;
		virtual std::error_code validate_fragment(std::string_view uri) const noexcept = 0;
	};

	known_uri_scheme const* query_uri_scheme(std::string_view scheme);

	template <typename CHAR_TYPE>
	class basic_uri_builder {

	public:

		basic_uri_builder(uri& uri);
		basic_uri_builder(uri& uri, known_uri_scheme const& scheme);
		basic_uri_builder(const basic_uri_builder&) = delete;
		basic_uri_builder& operator = (const basic_uri_builder&) = delete;
		~uri_builder();

		template <class Source>
		basic_uri_builder& scheme(const Source& scheme);

		template <class Source>
		basic_uri_builder& authority(const Source& authority);

		template <class Source>
		basic_uri_builder& authority(const Source& user_info, const Source& host, const Source& port);

		template <class Source>
		basic_uri_builder& user_info(const Source& user_info);

		template <class Source>
		basic_uri_builder& host(const Source& host);

		template <class Source>
		basic_uri_builder& port(const Source& port);

		template <class Source>
		basic_uri_builder& path(const Source& path);

		template <class Source>
		basic_uri_builder& query(const Source& query);

		template <class Source>
		basic_uri_builder& fragment(const Source& fragment);

	};

	using uri_builder = basic_uri_builder<char>;
	
	/// NOTE: https://www.iana.org/assignments/uri-schemes/uri-schemes.xhtml
	/// NOTE: https://en.wikipedia.org/wiki/List_of_URI_schemes
}