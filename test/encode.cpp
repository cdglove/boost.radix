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
#include <boost/radix/encode_iterator.hpp>
#include <boost/radix/static_ibitstream_lsb.hpp>
#include <boost/radix/static_ibitstream_msb.hpp>
#include <vector>

#include "common.hpp"

template <std::size_t Bits>
class msb_codec
    : public boost::radix::basic_codec<
          boost::radix::bits::to_alphabet_size<Bits>::value> {
public:
  msb_codec()
      : boost::radix::basic_codec<
            boost::radix::bits::to_alphabet_size<Bits>::value>(
            generate_alphabet(Bits)) {
  }
};

template <std::size_t Bits>
boost::radix::static_ibitstream_msb<Bits> get_segment_unpacker(
    msb_codec<Bits> const&) {
  return boost::radix::static_ibitstream_msb<Bits>();
}

template <std::size_t Bits>
class lsb_codec
    : public boost::radix::basic_codec<
          boost::radix::bits::to_alphabet_size<Bits>::value> {
public:
  lsb_codec()
      : boost::radix::basic_codec<
            boost::radix::bits::to_alphabet_size<Bits>::value>(
            generate_alphabet(Bits)) {
  }
};

template <std::size_t Bits>
boost::radix::static_ibitstream_lsb<Bits> get_segment_unpacker(
    lsb_codec<Bits> const&) {
  return boost::radix::static_ibitstream_lsb<Bits>();
}

template <std::size_t Bits, typename DataGenerator, typename Encoder>
void test_encode(DataGenerator data_generator, Encoder encoder) {
  std::vector<char_type> alphabet = generate_alphabet(Bits);
  std::vector<bits_type> data     = data_generator(Bits);
  std::string result;
  boost::radix::encode(
      data.begin(), data.end(), std::back_inserter(result), encoder);
  BOOST_TEST(std::equal(
      alphabet.begin(), alphabet.end(), result.begin(), is_equal_unsigned()));
}

template <std::size_t Bits, typename DataGenerator, typename Encoder>
void test_encoder(DataGenerator data_generator, Encoder codec) {
  std::vector<char_type> alphabet = generate_alphabet(Bits);
  std::vector<bits_type> data     = data_generator(Bits);
  std::string result;
  auto encoder = boost::radix::make_encoder(codec, std::back_inserter(result));
  encoder.append(data.begin(), data.end());
  encoder.resolve();
  BOOST_TEST(std::equal(
      alphabet.begin(), alphabet.end(), result.begin(), is_equal_unsigned()));
}

template <std::size_t Bits, typename Encoder>
void test_encode_part(Encoder codec) {
  std::vector<bits_type> data = generate_random_bytes(1024, (1 << Bits) - 1);
  std::string full_result;
  std::string partial_result;
  auto encoder =
      boost::radix::make_encoder(codec, std::back_inserter(full_result));
  encoder.append(data.begin(), data.end());
  encoder.resolve();
  encoder.reset(std::back_inserter(partial_result));
  std::size_t segment = 1;
  for(bits_type bits : data) {
    encoder.append(bits);
    // Ensure partial encoding is correct.
    if(encoder.bytes_written() ==
       (segment *
        boost::radix::codec_traits::packed_segment_size<Encoder>::value)) {
      ++segment;
      BOOST_TEST(std::equal(
          partial_result.begin(),
          partial_result.begin() + encoder.bytes_written(), full_result.begin(),
          is_equal_unsigned()));
    }
  }

  encoder.resolve();
  BOOST_TEST(full_result == partial_result);
}

template <std::size_t Bits, typename DataGenerator, typename Encoder>
void test_encode_iterator(DataGenerator data_generator, Encoder encoder) {
  std::vector<char_type> alphabet = generate_alphabet(Bits);
  std::vector<bits_type> data     = data_generator(Bits);
  std::string result;
  std::copy(
      data.begin(), data.end(),
      boost::radix::make_encode_iterator(encoder, std::back_inserter(result)));
  BOOST_TEST(std::equal(
      alphabet.begin(), alphabet.end(), result.begin(), is_equal_unsigned()));
}

template <std::size_t Bits>
void test_encode_msb() {
  test_encode<Bits>(generate_all_permutations_msb, msb_codec<Bits>());
}

template <std::size_t Bits>
void test_encode_lsb() {
  test_encode<Bits>(generate_all_permutations_lsb, lsb_codec<Bits>());
}

BOOST_AUTO_TEST_CASE(encode_one_bit_msb) {
  test_encode_msb<1>();
}

BOOST_AUTO_TEST_CASE(encode_two_bit_msb) {
  test_encode_msb<2>();
}

BOOST_AUTO_TEST_CASE(encode_three_bit_msb) {
  test_encode_msb<3>();
}

BOOST_AUTO_TEST_CASE(encode_four_bit_msb) {
  test_encode_msb<4>();
}

BOOST_AUTO_TEST_CASE(encode_five_bit_msb) {
  test_encode_msb<5>();
}

BOOST_AUTO_TEST_CASE(encode_six_bit_msb) {
  test_encode_msb<6>();
}

BOOST_AUTO_TEST_CASE(encode_one_bit_lsb) {
  test_encode_lsb<1>();
}

BOOST_AUTO_TEST_CASE(encode_two_bit_lsb) {
  test_encode_lsb<2>();
}

BOOST_AUTO_TEST_CASE(encode_three_bit_lsb) {
  test_encode_lsb<3>();
}

BOOST_AUTO_TEST_CASE(encode_four_bit_lsb) {
  test_encode_lsb<4>();
}

BOOST_AUTO_TEST_CASE(encode_five_bit_lsb) {
  test_encode_lsb<5>();
}

BOOST_AUTO_TEST_CASE(encode_six_bit_lsb) {
  test_encode_lsb<6>();
}

BOOST_AUTO_TEST_CASE(encode_seven_bit_msb) {
  test_encode_lsb<7>();
}

BOOST_AUTO_TEST_CASE(encode_seven_bit_lsb) {
  test_encode_lsb<7>();
}

BOOST_AUTO_TEST_CASE(encoder_one_bit_msb) {
  test_encoder<1>(generate_all_permutations_msb, msb_codec<1>());
}

BOOST_AUTO_TEST_CASE(encoder_two_bit_msb) {
  test_encoder<2>(generate_all_permutations_msb, msb_codec<2>());
}

BOOST_AUTO_TEST_CASE(encoder_three_bit_msb) {
  test_encoder<3>(generate_all_permutations_msb, msb_codec<3>());
}

BOOST_AUTO_TEST_CASE(encoder_four_bit_msb) {
  test_encoder<4>(generate_all_permutations_msb, msb_codec<4>());
}

BOOST_AUTO_TEST_CASE(encoder_five_bit_msb) {
  test_encoder<5>(generate_all_permutations_msb, msb_codec<5>());
}

BOOST_AUTO_TEST_CASE(encoder_six_bit_msb) {
  test_encoder<6>(generate_all_permutations_msb, msb_codec<6>());
}

BOOST_AUTO_TEST_CASE(encoder_seven_bit_msb) {
  test_encoder<7>(generate_all_permutations_msb, msb_codec<7>());
}

BOOST_AUTO_TEST_CASE(encode_iterator_one_bit_msb) {
  test_encode_iterator<1>(generate_all_permutations_msb, msb_codec<1>());
}

BOOST_AUTO_TEST_CASE(encode_iterator_two_bit_msb) {
  test_encode_iterator<2>(generate_all_permutations_msb, msb_codec<2>());
}

BOOST_AUTO_TEST_CASE(encode_iterator_three_bit_msb) {
  test_encode_iterator<3>(generate_all_permutations_msb, msb_codec<3>());
}

BOOST_AUTO_TEST_CASE(encode_iterator_four_bit_msb) {
  test_encode_iterator<4>(generate_all_permutations_msb, msb_codec<4>());
}

BOOST_AUTO_TEST_CASE(encode_iterator_five_bit_msb) {
  test_encode_iterator<5>(generate_all_permutations_msb, msb_codec<5>());
}

BOOST_AUTO_TEST_CASE(encode_iterator_six_bit_msb) {
  test_encode_iterator<6>(generate_all_permutations_msb, msb_codec<6>());
}

BOOST_AUTO_TEST_CASE(encode_iterator_seven_bit_msb) {
  test_encode_iterator<7>(generate_all_permutations_msb, msb_codec<7>());
}

BOOST_AUTO_TEST_CASE(encode_part_one_bit_msb) {
  test_encode_part<1>(msb_codec<1>());
}

BOOST_AUTO_TEST_CASE(encode_part_two_bit_msb) {
  test_encode_part<2>(msb_codec<2>());
}

BOOST_AUTO_TEST_CASE(encode_part_three_bit_msb) {
  test_encode_part<3>(msb_codec<3>());
}

BOOST_AUTO_TEST_CASE(encode_part_four_bit_msb) {
  test_encode_part<4>(msb_codec<4>());
}

BOOST_AUTO_TEST_CASE(encode_part_five_bit_msb) {
  test_encode_part<5>(msb_codec<5>());
}

BOOST_AUTO_TEST_CASE(encode_part_six_bit_msb) {
  test_encode_part<6>(msb_codec<6>());
}

BOOST_AUTO_TEST_CASE(encode_part_seven_bit_msb) {
  test_encode_part<7>(msb_codec<7>());
}