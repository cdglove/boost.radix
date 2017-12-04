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

template<typename T>
T get_expected_bits(int idx, int num_bits)
{
    T ret = 0;
    int start_bit = (idx * num_bits) % 2;
    for(int i = 0; i < num_bits; ++i)
    {
        ret |= ((i + start_bit) % 2) << i;
    }
    return ret;
}

std::vector<char> get_bit_buffer(int count)
{
    std::vector<char> buf;
    for(int i = 0; i < count; ++i)
    {
        buf.push_back(get_expected_bits<char>(i, 8));
    }
    return buf;
}

BOOST_AUTO_TEST_CASE(encode_chars_binary)
{
    std::vector<char> buf = get_bit_buffer(4);
    std::string result;
    boost::radix::encode(buf.begin(), buf.end(), std::back_inserter(result), boost::radix::alphabet<2>("01"));
    BOOST_TEST(result == "01010101010101010101010101010101");
}

BOOST_AUTO_TEST_CASE(encode_chars_hex)
{
    std::vector<unsigned char> buf { 0x01, 0x10, 0x5a, 0xa5, 0xff };
    std::string result;
    boost::radix::encode(buf.begin(), buf.end(), std::back_inserter(result), boost::radix::alphabet<16>("0123456789ABCDEF"));

    // Result is switched because little endian representation of 0x01
    BOOST_TEST(result == "1001A55AFF");
}