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
#include <boost/radix/basic_codec.hpp>
#include <boost/radix/encode.hpp>
#include <boost/radix/static_ibitstream_lsb.hpp>
#include <boost/radix/static_ibitstream_msb.hpp>
#include <vector>

#include "common.hpp"

template <std::size_t Bits>
class msb_codec
    : public boost::radix::basic_codec<
          boost::radix::bits::to_alphabet_size<Bits>::value>
{
public:
    msb_codec()
        : boost::radix::basic_codec<
              boost::radix::bits::to_alphabet_size<Bits>::value>(
              generate_alphabet(Bits))
    {}
};

template <std::size_t Bits>
boost::radix::static_ibitstream_msb<Bits>
get_segment_unpacker(msb_codec<Bits> const&)
{
    return boost::radix::static_ibitstream_msb<Bits>();
}

template <std::size_t Bits>
class lsb_codec
    : public boost::radix::basic_codec<
          boost::radix::bits::to_alphabet_size<Bits>::value>
{
public:
    lsb_codec()
        : boost::radix::basic_codec<
              boost::radix::bits::to_alphabet_size<Bits>::value>(
              generate_alphabet(Bits))
    {}
};

template <std::size_t Bits>
boost::radix::static_ibitstream_lsb<Bits>
get_segment_unpacker(lsb_codec<Bits> const&)
{
    return boost::radix::static_ibitstream_lsb<Bits>();
}

template <std::size_t Bits, typename DataGenerator, typename Encoder>
void test_encode(DataGenerator data_generator, Encoder encoder)
{
    std::vector<char_type> alphabet = generate_alphabet(Bits);
    std::vector<bits_type> data     = data_generator(Bits);
    std::string result;
    boost::radix::encode(
        data.begin(), data.end(), std::back_inserter(result), encoder);
    BOOST_TEST(std::equal(
        alphabet.begin(), alphabet.end(), result.begin(), is_equal_unsigned()));
}

template <std::size_t Bits>
void test_encode_msb()
{
    test_encode<Bits>(generate_all_permutations_msb, msb_codec<Bits>());
}

template <std::size_t Bits>
void test_encode_lsb()
{
    test_encode<Bits>(generate_all_permutations_lsb, lsb_codec<Bits>());
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
