//
// test/decode.cpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE TestDecode
#include <boost/test/unit_test.hpp>

#include <array>
#include <boost/radix/alphabet.hpp>
#include <boost/radix/decode.hpp>
#include <boost/radix/static_obitstream_lsb.hpp>
#include <vector>

#include "common.hpp"

template <std::size_t Bits, typename DataGenerator, typename Decoder>
void test_decode(DataGenerator data_generator, Decoder decoder)
{
    std::vector<char_type> alphabet = generate_alphabet(Bits);
    std::vector<bits_type> data     = data_generator(Bits);
    std::vector<bits_type> result;
    boost::radix::decode(
        alphabet.begin(), alphabet.end(), std::back_inserter(result),
        boost::radix::alphabet<1 << Bits>(alphabet.begin(), alphabet.end()),
        decoder);
    BOOST_TEST(boost::equal(data, result, is_equal_unsigned()));
}

template <std::size_t Bits>
void test_decode_msb()
{
    test_decode<Bits>(
        generate_all_permutations_msb,
        boost::radix::static_obitstream_msb<Bits>());
}

template <std::size_t Bits>
void test_decode_lsb()
{
    test_decode<Bits>(
        generate_all_permutations_lsb,
        boost::radix::static_obitstream_lsb<Bits>());
}

BOOST_AUTO_TEST_CASE(decode_one_bit_msb)
{
    test_decode_lsb<1>();
}

BOOST_AUTO_TEST_CASE(decode_one_bit_lsb)
{
    test_decode_lsb<1>();
}

BOOST_AUTO_TEST_CASE(decode_two_bit_msb)
{
    test_decode_msb<2>();
}

BOOST_AUTO_TEST_CASE(decode_two_bit_lsb)
{
    test_decode_lsb<2>();
}

BOOST_AUTO_TEST_CASE(decode_three_bit_msb)
{
    test_decode_msb<3>();
}

BOOST_AUTO_TEST_CASE(decode_three_bit_lsb)
{
    test_decode_lsb<3>();
}

BOOST_AUTO_TEST_CASE(decode_four_bit_msb)
{
    test_decode_msb<4>();
}

BOOST_AUTO_TEST_CASE(decode_four_bit_lsb)
{
    test_decode_lsb<4>();
}

BOOST_AUTO_TEST_CASE(decode_five_bit_msb)
{
    test_decode_msb<5>();
}

BOOST_AUTO_TEST_CASE(decode_five_bit_lsb)
{
    test_decode_lsb<5>();
}

BOOST_AUTO_TEST_CASE(decode_six_bit_msb)
{
    test_decode_msb<6>();
}

BOOST_AUTO_TEST_CASE(decode_six_bit_lsb)
{
    test_decode_lsb<6>();
}

BOOST_AUTO_TEST_CASE(decode_seven_bit_msb)
{
    test_decode_msb<7>();
}

BOOST_AUTO_TEST_CASE(decode_seven_bit_lsb)
{
    test_decode_lsb<7>();
}
