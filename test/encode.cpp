//
// test/ibstream.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE TestEncode
#include <boost/test/unit_test.hpp>

#include <array>
#include <boost/radix/alphabet.hpp>
#include <boost/radix/encode.hpp>
#include <boost/radix/static_ibitstream_lsb.hpp>
#include <vector>

#include "common.hpp"

BOOST_AUTO_TEST_CASE(encode_one_bit_msb)
{
    std::vector<bits_type> buf = generate_01_bit_pattern_bytes(1);
    std::string result;
    boost::radix::encode(
        buf.begin(), buf.end(), std::back_inserter(result),
        boost::radix::alphabet<2>("01"));
    BOOST_TEST(result == "01010101");
}

BOOST_AUTO_TEST_CASE(encode_one_bit_lsb)
{
    std::vector<bits_type> buf = generate_01_bit_pattern_bytes(1);
    std::string result;
    boost::radix::encode(
        buf.begin(), buf.end(), std::back_inserter(result),
        boost::radix::alphabet<2>("01"),
        boost::radix::static_ibitstream_lsb<1>());
    BOOST_TEST(result == "10101010");
}

BOOST_AUTO_TEST_CASE(encode_two_bit_msb)
{
    std::vector<bits_type> buf = generate_01_bit_pattern_bytes(1);
    std::string result;
    boost::radix::encode(
        buf.begin(), buf.end(), std::back_inserter(result),
        boost::radix::alphabet<4>("0123"));
    BOOST_TEST(result == "1111");
}

BOOST_AUTO_TEST_CASE(encode_two_bit_lsb)
{
    std::vector<bits_type> buf = generate_01_bit_pattern_bytes(1);
    std::string result;
    boost::radix::encode(
        buf.begin(), buf.end(), std::back_inserter(result),
        boost::radix::alphabet<4>("0123"),
        boost::radix::static_ibitstream_lsb<2>());
    BOOST_TEST(result == "1111");
}

BOOST_AUTO_TEST_CASE(encode_three_bit_msb)
{
    std::vector<bits_type> buf = generate_01_bit_pattern_bytes(3 /* lcm(3,8) / 8 */);
    std::string result;
    boost::radix::encode(
        buf.begin(), buf.end(), std::back_inserter(result),
        boost::radix::alphabet<8>("01234567"));
    BOOST_TEST(result == "25252525");
}

BOOST_AUTO_TEST_CASE(encode_three_bit_lsb)
{
    std::vector<bits_type> buf = generate_01_bit_pattern_bytes(1);
    std::string result;
    boost::radix::encode(
        buf.begin(), buf.end(), std::back_inserter(result),
        boost::radix::alphabet<8>("01234567"),
        boost::radix::static_ibitstream_lsb<2>());
    BOOST_TEST(result == "52525252");
}

BOOST_AUTO_TEST_CASE(encode_four_bit_msb)
{
    std::vector<bits_type> buf = generate_01_bit_pattern_bytes(3 /* lcm(3,8) / 8 */);
    std::string result;
    boost::radix::encode(
        buf.begin(), buf.end(), std::back_inserter(result),
        boost::radix::alphabet<16>("0123456789ABCDEF"));
    BOOST_TEST(result == "55");
}

BOOST_AUTO_TEST_CASE(encode_four_bit_lsb)
{
    std::vector<bits_type> buf = generate_01_bit_pattern_bytes(1);
    std::string result;
    boost::radix::encode(
        buf.begin(), buf.end(), std::back_inserter(result),
        boost::radix::alphabet<16>("0123456789ABCDEF"),
        boost::radix::static_ibitstream_lsb<4>());
    BOOST_TEST(result == "AA");
}