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

#include <boost/radix/encode.hpp>
#include <boost/radix/alphabet.hpp>
#include <vector>
#include <array>

#include "common.hpp"

BOOST_AUTO_TEST_CASE(encode_one_bit)
{
    std::vector<bits_type> buf = generate_01_bit_pattern_bytes(1);
    std::string result;
    boost::radix::encode(buf.begin(), buf.end(), std::back_inserter(result), boost::radix::alphabet<2>("01"));
    BOOST_TEST(result == "01010101");
}
//
//BOOST_AUTO_TEST_CASE(encode_chars_hex)
//{
//    std::vector<unsigned char> buf { 0x01, 0x10, 0x5a, 0xa5, 0xff };
//    std::string result;
//    boost::radix::encode(buf.begin(), buf.end(), std::back_inserter(result), boost::radix::alphabet<16>("0123456789ABCDEF"));
//
//    // Result is switched because little endian representation of 0x01
//    BOOST_TEST(result == "1001A55AFF");
//}