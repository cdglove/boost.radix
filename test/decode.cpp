//
// test/decode.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE TestDecode
#include <boost/test/unit_test.hpp>

#include <boost/radix/alphabet.hpp>
#include <boost/radix/decode.hpp>
#include <vector>
#include <array>

//template<typename T>
//T get_expected_bits(int idx, int num_bits)
//{
//    T ret = 0;
//    int start_bit = (idx * num_bits) % 2;
//    for(int i = 0; i < num_bits; ++i)
//    {
//        ret |= ((i + start_bit) % 2) << i;
//    }
//    return ret;
//}
//
//BOOST_AUTO_TEST_CASE(decode_chars_binary)
//{
//    std::vector<char> buf;
//    std::string str("01010101010101010101010101010101");
//    std::vector<char> result;
//    boost::radix::alphabet<2> binary("01");
//    result.resize(boost::radix::decoded_size(str.length(), binary));
//    boost::radix::decode(str.begin(), str.end(), result.begin(), binary);
//    BOOST_TEST(std::count(result.begin(), result.end(), get_expected_bits<char>(0, 8)) == result.size());
//}
//
//BOOST_AUTO_TEST_CASE(decode_chars_hex)
//{
//    std::vector<char> buf;
//    std::string str("0011AAFF1E");
//    std::vector<unsigned char> result;
//    boost::radix::alphabet<2> hex("0123456789ABCDEF");
//    result.resize(boost::radix::decoded_size(str.length(), hex));
//    boost::radix::decode(str.begin(), str.end(), result.begin(), hex);
//    BOOST_TEST(result[0] == 0x00U);
//    BOOST_TEST(result[1] == 0x11U);
//    BOOST_TEST(result[2] == 0xAAU);
//    BOOST_TEST(result[3] == 0xFFU);
//    BOOST_TEST(result[4] == 0xE1U);
//}
//
//BOOST_AUTO_TEST_CASE(decode_chars_dec)
//{
//    std::vector<char> buf;
//    std::string str("001122345689");
//    std::vector<unsigned char> result;
//    boost::radix::alphabet<10> dec("0123456789");
//    result.resize(boost::radix::decoded_size(str.length(), dec));
//    boost::radix::decode(str.begin(), str.end(), result.begin(), dec);
//}