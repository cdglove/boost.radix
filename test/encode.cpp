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

template <std::size_t Bits>
void test_encode_msb()
{
    std::vector<char_type> alphabet = generate_alphabet(Bits);
    std::vector<bits_type> data     = generate_all_permutations_msb(Bits);
    std::string result;
    boost::radix::encode(
        data.begin(), data.end(), std::back_inserter(result),
        boost::radix::alphabet<1 << Bits>(alphabet.begin(), alphabet.end()),
        boost::radix::static_ibitstream_msb<Bits>());
    BOOST_TEST(std::equal(
        alphabet.begin(), alphabet.end(), result.begin(), is_equal_unsigned()));
}

template <std::size_t Bits>
void test_encode_lsb()
{
    std::vector<char_type> alphabet = generate_alphabet(Bits);
    std::vector<bits_type> data     = generate_all_permutations_lsb(Bits);
    std::string result;
    boost::radix::encode(
        data.begin(), data.end(), std::back_inserter(result),
        boost::radix::alphabet<1 << Bits>(alphabet.begin(), alphabet.end()),
        boost::radix::static_ibitstream_lsb<Bits>());
    BOOST_TEST(std::equal(
        alphabet.begin(), alphabet.end(), result.begin(), is_equal_unsigned()));
}

BOOST_AUTO_TEST_CASE(encode_one_bit_msb)
{
    test_encode_msb<1>();
}

BOOST_AUTO_TEST_CASE(encode_one_bit_lsb)
{
    test_encode_lsb<1>();
}

BOOST_AUTO_TEST_CASE(encode_two_bit_msb)
{
    test_encode_msb<2>();
}

BOOST_AUTO_TEST_CASE(encode_two_bit_lsb)
{
    test_encode_lsb<2>();
}

BOOST_AUTO_TEST_CASE(encode_three_bit_msb)
{
    test_encode_msb<3>();
}

BOOST_AUTO_TEST_CASE(encode_three_bit_lsb)
{
    test_encode_lsb<3>();
}

BOOST_AUTO_TEST_CASE(encode_four_bit_msb)
{
    test_encode_msb<4>();
}

BOOST_AUTO_TEST_CASE(encode_four_bit_lsb)
{
    test_encode_lsb<4>();
}

BOOST_AUTO_TEST_CASE(encode_five_bit_msb)
{
    test_encode_msb<5>();
}

BOOST_AUTO_TEST_CASE(encode_five_bit_lsb)
{
    test_encode_lsb<5>();
}

BOOST_AUTO_TEST_CASE(encode_six_bit_msb)
{
    test_encode_msb<6>();
}

BOOST_AUTO_TEST_CASE(encode_six_bit_lsb)
{
    test_encode_lsb<6>();
}

BOOST_AUTO_TEST_CASE(encode_seven_bit_msb)
{
    test_encode_lsb<7>();
}

BOOST_AUTO_TEST_CASE(encode_seven_bit_lsb)
{
    test_encode_lsb<7>();
}
