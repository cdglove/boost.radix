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
template <typename Range, typename Codec>
std::string encode(Range const& input, Codec const& codec)
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
std::string encode_string(boost::string_view input, Codec const& codec)
{
    return encode(boost::make_iterator_range(input.begin(), input.end()), codec);
}

}} // namespace boost::radix

BOOST_AUTO_TEST_CASE(base64)
{
    boost::radix::codecs::rfc4648::base64 codec;

    BOOST_TEST(boost::radix::encode_string("", codec) == "");
    BOOST_TEST(boost::radix::encode_string("f", codec) == "Zg==");
    BOOST_TEST(boost::radix::encode_string("fo", codec) == "Zm8=");
    BOOST_TEST(boost::radix::encode_string("foo", codec) == "Zm9v");
    BOOST_TEST(boost::radix::encode_string("foob", codec) == "Zm9vYg==");
    BOOST_TEST(boost::radix::encode_string("fooba", codec) == "Zm9vYmE=");
    BOOST_TEST(boost::radix::encode_string("foobar", codec) == "Zm9vYmFy");
}

BOOST_AUTO_TEST_CASE(base64url)
{}

BOOST_AUTO_TEST_CASE(base32)
{}

BOOST_AUTO_TEST_CASE(base32hex)
{}

BOOST_AUTO_TEST_CASE(base16)
{}
