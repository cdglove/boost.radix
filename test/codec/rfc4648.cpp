//
// test/codec/rfc4648.cpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE TestRfc4648
#include <boost/test/unit_test.hpp>

#include <boost/radix/codecs/rfc4648/base16.hpp>
#include <boost/radix/codecs/rfc4648/base32.hpp>
#include <boost/radix/codecs/rfc4648/base32hex.hpp>
#include <boost/radix/codecs/rfc4648/base64.hpp>
#include <boost/radix/codecs/rfc4648/base64url.hpp>
#include <boost/radix/decode.hpp>
#include <boost/radix/encode.hpp>

// Test vectors taken from https://tools.ietf.org/html/rfc4648

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
namespace boost { namespace radix {
// -----------------------------------------------------------------------------
//
template <typename Range, typename OutputIterator, typename Codec>
void encode(Range const& input, OutputIterator out, Codec const& codec)
{
    using boost::radix::adl::get_segment_unpacker;
    encode(
        boost::begin(input), boost::end(input), out, codec,
        get_segment_unpacker(codec));
}

// -----------------------------------------------------------------------------
//
template <typename CharType, typename Codec>
std::string encode(boost::basic_string_view<CharType> input, Codec const& codec)
{
    using boost::radix::adl::get_segment_unpacker;
    std::string result;
    result.resize(encoded_size(
        std::distance(boost::begin(input), boost::end(input)), codec));

    encode(
        boost::begin(input), boost::end(input), result.begin(), codec,
        get_segment_unpacker(codec));
    while(!result.empty() && result.back() == '\0')
        result.pop_back();
    return result;
}

// -----------------------------------------------------------------------------
//
template <typename Codec>
std::string encode(char const* input, Codec const& codec)
{
    return encode(boost::string_view(input), codec);
}

}} // namespace boost::radix

BOOST_AUTO_TEST_CASE(base64)
{
    boost::radix::codecs::rfc4648::base64 codec;

    BOOST_TEST(boost::radix::encode("", codec) == "");
    BOOST_TEST(boost::radix::encode("f", codec) == "Zg==");
    BOOST_TEST(boost::radix::encode("fo", codec) == "Zm8=");
    BOOST_TEST(boost::radix::encode("foo", codec) == "Zm9v");
    BOOST_TEST(boost::radix::encode("foob", codec) == "Zm9vYg==");
    BOOST_TEST(boost::radix::encode("fooba", codec) == "Zm9vYmE=");
    BOOST_TEST(boost::radix::encode("foobar", codec) == "Zm9vYmFy");
}

BOOST_AUTO_TEST_CASE(base64url)
{   
    boost::radix::codecs::rfc4648::base64url codec;

    BOOST_TEST(boost::radix::encode("", codec) == "");
    BOOST_TEST(boost::radix::encode("f", codec) == "Zg==");
    BOOST_TEST(boost::radix::encode("fo", codec) == "Zm8=");
    BOOST_TEST(boost::radix::encode("foo", codec) == "Zm9v");
    BOOST_TEST(boost::radix::encode("foob", codec) == "Zm9vYg==");
    BOOST_TEST(boost::radix::encode("fooba", codec) == "Zm9vYmE=");
    BOOST_TEST(boost::radix::encode("foobar", codec) == "Zm9vYmFy");
}

BOOST_AUTO_TEST_CASE(base32)
{
    boost::radix::codecs::rfc4648::base32 codec;

    BOOST_TEST(boost::radix::encode("", codec) == "");
    BOOST_TEST(boost::radix::encode("f", codec) == "MY======");
    BOOST_TEST(boost::radix::encode("fo", codec) == "MZXQ====");
    BOOST_TEST(boost::radix::encode("foo", codec) == "MZXW6===");
    BOOST_TEST(boost::radix::encode("foob", codec) == "MZXW6YQ=");
    BOOST_TEST(boost::radix::encode("fooba", codec) == "MZXW6YTB");
    BOOST_TEST(boost::radix::encode("foobar", codec) == "MZXW6YTBOI======");
}

BOOST_AUTO_TEST_CASE(base32hex)
{
    boost::radix::codecs::rfc4648::base32hex codec;

    BOOST_TEST(boost::radix::encode("", codec) == "");
    BOOST_TEST(boost::radix::encode("f", codec) == "CO======");
    BOOST_TEST(boost::radix::encode("fo", codec) == "CPNG====");
    BOOST_TEST(boost::radix::encode("foo", codec) == "CPNMU===");
    BOOST_TEST(boost::radix::encode("foob", codec) == "CPNMUOG=");
    BOOST_TEST(boost::radix::encode("fooba", codec) == "CPNMUOJ1");
    BOOST_TEST(boost::radix::encode("foobar", codec) == "CPNMUOJ1E8======");
}

BOOST_AUTO_TEST_CASE(base16)
{
    boost::radix::codecs::rfc4648::base16 codec;

    BOOST_TEST(boost::radix::encode("", codec) == "");
    BOOST_TEST(boost::radix::encode("f", codec) == "66");
    BOOST_TEST(boost::radix::encode("fo", codec) == "666F");
    BOOST_TEST(boost::radix::encode("foo", codec) == "666F6F");
    BOOST_TEST(boost::radix::encode("foob", codec) == "666F6F62");
    BOOST_TEST(boost::radix::encode("fooba", codec) == "666F6F6261");
    BOOST_TEST(boost::radix::encode("foobar", codec) == "666F6F626172");
}
