//
// test/ibstream.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE TestBitstreams
#include <boost/test/unit_test.hpp>

#include "common.hpp"
#include <boost/array.hpp>
#include <boost/radix/detail/bits.hpp>
#include <boost/radix/static_ibitstream_lsb.hpp>
#include <boost/radix/static_ibitstream_msb.hpp>
#include <boost/radix/static_obitstream_lsb.hpp>
#include <boost/radix/static_obitstream_msb.hpp>
#include <vector>

// So we can easily call apis taking iterator refs.
template <typename Function, typename Arg1, typename Arg2>
void call(Function f, Arg1 arg1, Arg2 arg2) {
  f(arg1, arg2);
}

template <std::size_t Bits>
static void check_static_istream_lsb(std::size_t num_segments) {
  std::size_t const packed_segment_size =
      boost::radix::bits::to_packed_segment_size<Bits>::value;
  std::size_t const unpacked_segment_size =
      boost::radix::bits::to_unpacked_segment_size<Bits>::value;

  std::size_t max_value = ~(std::size_t(~0) << Bits);

  // Checking every bit combination in every position would be possible, though
  // it would take a long time, so instead we check to make sure we can extract
  // every bit combination from every posision, with all zeros in one pass and
  // all ones in a second pass.
  std::vector<bits_type> packed(packed_segment_size);
  std::vector<bits_type> unpacked(unpacked_segment_size);

  for(std::size_t i = 0; i < max_value; ++i) {
    std::size_t offset = 0;
    bits_type value    = bits_type(i);
    for(std::size_t j = 0; j < unpacked_segment_size; ++j) {
      std::fill(packed.begin(), packed.end(), 0);
      set_bits_lsb(packed, value, offset, Bits);
      call(
          boost::radix::static_ibitstream_lsb<Bits>(), packed.begin(),
          unpacked.begin());
      BOOST_TEST(unpacked[j] == value);

      std::fill(packed.begin(), packed.end(), bits_type(~0));
      set_bits_lsb(packed, value, offset, Bits);
      call(
          boost::radix::static_ibitstream_lsb<Bits>(), packed.begin(),
          unpacked.begin());
      BOOST_TEST(unpacked[j] == value);

      offset += Bits;
    }
  }
}

template <std::size_t Bits>
static void check_static_istream_msb(std::size_t num_segments) {
  std::size_t const packed_segment_size =
      boost::radix::bits::to_packed_segment_size<Bits>::value;
  std::size_t const unpacked_segment_size =
      boost::radix::bits::to_unpacked_segment_size<Bits>::value;

  std::size_t max_value = ~(std::size_t(~0) << Bits);

  // Checking every bit combination in every position would be possible, though
  // it would take a long time, so instead we check to make sure we can extract
  // every bit combination from every posision, with all zeros in one pass and
  // all ones in a second pass.
  std::vector<bits_type> packed(packed_segment_size);
  std::vector<bits_type> unpacked(unpacked_segment_size);

  for(std::size_t i = 0; i < max_value; ++i) {
    std::size_t offset = 0;
    bits_type value    = bits_type(i);
    for(std::size_t j = 0; j < unpacked_segment_size; ++j) {
      std::fill(packed.begin(), packed.end(), 0);
      set_bits_msb(packed, value, offset, Bits);
      call(
          boost::radix::static_ibitstream_msb<Bits>(), packed.begin(),
          unpacked.begin());
      BOOST_TEST(unpacked[j] == value);

      std::fill(packed.begin(), packed.end(), bits_type(~0));
      set_bits_msb(packed, value, offset, Bits);
      call(
          boost::radix::static_ibitstream_msb<Bits>(), packed.begin(),
          unpacked.begin());
      BOOST_TEST(unpacked[j] == value);

      offset += Bits;
    }
  }
}

template <std::size_t Bits>
static void check_static_ostream_lsb(std::size_t num_segments) {
  std::size_t const packed_segment_size =
      boost::radix::bits::to_packed_segment_size<Bits>::value;
  std::size_t const unpacked_segment_size =
      boost::radix::bits::to_unpacked_segment_size<Bits>::value;

  std::size_t max_value = ~(std::size_t(~0) << Bits);

  // Checking every bit combination in every position would be possible, though
  // it would take a long time, so instead we check to make sure we can extract
  // every bit combination from every posision, with all zeros in one pass and
  // all ones in a second pass.
  std::vector<bits_type> packed(packed_segment_size);
  std::vector<bits_type> unpacked(unpacked_segment_size);

  for(std::size_t i = 0; i < max_value; ++i) {
    std::size_t offset = 0;
    bits_type value    = bits_type(i);
    for(std::size_t j = 0; j < unpacked_segment_size; ++j) {
      std::fill(unpacked.begin(), unpacked.end(), 0);
      unpacked[j] = value;
      boost::radix::static_obitstream_lsb<Bits>()(unpacked, packed);
      BOOST_TEST(get_bits_lsb(packed, offset, Bits) == value);

      std::fill(unpacked.begin(), unpacked.end(), max_value);
      unpacked[j] = value;
      boost::radix::static_obitstream_lsb<Bits>()(unpacked, packed);
      BOOST_TEST(get_bits_lsb(packed, offset, Bits) == value);

      offset += Bits;
    }
  }
}

template <std::size_t Bits>
static void check_static_ostream_msb(std::size_t num_segments) {
  std::size_t const packed_segment_size =
      boost::radix::bits::to_packed_segment_size<Bits>::value;
  std::size_t const unpacked_segment_size =
      boost::radix::bits::to_unpacked_segment_size<Bits>::value;

  std::size_t max_value = ~(std::size_t(~0) << Bits);

  // Checking every bit combination in every position would be possible, though
  // it would take a long time, so instead we check to make sure we can extract
  // every bit combination from every posision, with all zeros in one pass and
  // all ones in a second pass.
  std::vector<bits_type> packed(packed_segment_size);
  std::vector<bits_type> unpacked(unpacked_segment_size);

  for(std::size_t i = 0; i < max_value; ++i) {
    std::size_t offset = 0;
    bits_type value    = bits_type(i);
    for(std::size_t j = 0; j < unpacked_segment_size; ++j) {
      std::fill(unpacked.begin(), unpacked.end(), 0);
      unpacked[j] = value;
      boost::radix::static_obitstream_msb<Bits>()(unpacked, packed);
      BOOST_TEST(get_bits_msb(packed, offset, Bits) == value);

      std::fill(unpacked.begin(), unpacked.end(), max_value);
      unpacked[j] = value;
      boost::radix::static_obitstream_msb<Bits>()(unpacked, packed);
      BOOST_TEST(get_bits_msb(packed, offset, Bits) == value);

      offset += Bits;
    }
  }
}

template <std::size_t Bits>
static void check_static_iostream_lsb(std::size_t num_segments) {
  std::size_t const packed_segment_size =
      boost::radix::bits::to_packed_segment_size<Bits>::value;
  std::size_t const unpacked_segment_size =
      boost::radix::bits::to_unpacked_segment_size<Bits>::value;

  std::size_t max_value = ~(std::size_t(~0) << Bits);

  // Checking every bit combination in every position would be possible, though
  // it would take a long time, so instead we check to make sure we can extract
  // every bit combination from every posision, with all zeros in one pass and
  // all ones in a second pass.
  std::vector<bits_type> packed(packed_segment_size);
  std::vector<bits_type> unpacked_source(unpacked_segment_size);
  std::vector<bits_type> unpacked_result(unpacked_segment_size);

  for(std::size_t i = 0; i < max_value; ++i) {
    std::size_t offset = 0;
    bits_type value    = bits_type(i);
    for(std::size_t j = 0; j < unpacked_segment_size; ++j) {
      std::fill(unpacked_source.begin(), unpacked_source.end(), 0);
      unpacked_source[j] = value;
      boost::radix::static_obitstream_lsb<Bits>()(unpacked_source, packed);
      call(
          boost::radix::static_ibitstream_lsb<Bits>(), packed.begin(),
          unpacked_result.begin());
      BOOST_TEST(unpacked_source == unpacked_result);

      std::fill(unpacked_source.begin(), unpacked_source.end(), max_value);
      unpacked_source[j] = value;
      boost::radix::static_obitstream_lsb<Bits>()(unpacked_source, packed);
      call(
          boost::radix::static_ibitstream_lsb<Bits>(), packed.begin(),
          unpacked_result.begin());
      BOOST_TEST(unpacked_source == unpacked_result);

      offset += Bits;
    }
  }
}

template <std::size_t Bits>
static void check_static_iostream_msb(std::size_t num_segments) {
  std::size_t const packed_segment_size =
      boost::radix::bits::to_packed_segment_size<Bits>::value;
  std::size_t const unpacked_segment_size =
      boost::radix::bits::to_unpacked_segment_size<Bits>::value;

  std::size_t max_value = ~(std::size_t(~0) << Bits);

  // Checking every bit combination in every position would be possible, though
  // it would take a long time, so instead we check to make sure we can extract
  // every bit combination from every posision, with all zeros in one pass and
  // all ones in a second pass.
  std::vector<bits_type> packed(packed_segment_size);
  std::vector<bits_type> unpacked_source(unpacked_segment_size);
  std::vector<bits_type> unpacked_result(unpacked_segment_size);

  for(std::size_t i = 0; i < max_value; ++i) {
    std::size_t offset = 0;
    bits_type value    = bits_type(i);
    for(std::size_t j = 0; j < unpacked_segment_size; ++j) {
      std::fill(unpacked_source.begin(), unpacked_source.end(), 0);
      unpacked_source[j] = value;
      boost::radix::static_obitstream_msb<Bits>()(unpacked_source, packed);
      call(
          boost::radix::static_ibitstream_msb<Bits>(), packed.begin(),
          unpacked_result.begin());
      BOOST_TEST(unpacked_source == unpacked_result);

      std::fill(unpacked_source.begin(), unpacked_source.end(), max_value);
      unpacked_source[j] = value;
      boost::radix::static_obitstream_msb<Bits>()(unpacked_source, packed);
      call(
          boost::radix::static_ibitstream_msb<Bits>(), packed.begin(),
          unpacked_result.begin());
      BOOST_TEST(unpacked_source == unpacked_result);

      offset += Bits;
    }
  }
}

BOOST_AUTO_TEST_CASE(static_ibitstream_lsb) {
  // Testing 1 segment lets us catch buffer overflows.
  // Testing 10 gives us more data and tests the segment boundaries.
  boost::array<std::size_t, 2> segments = {1, 10};
  for(int i = 0; i < segments.size(); ++i) {
    check_static_istream_lsb<1>(segments[i]);
    check_static_istream_lsb<2>(segments[i]);
    check_static_istream_lsb<3>(segments[i]);
    check_static_istream_lsb<4>(segments[i]);
    check_static_istream_lsb<5>(segments[i]);
    check_static_istream_lsb<6>(segments[i]);
    check_static_istream_lsb<7>(segments[i]);
  }
}

BOOST_AUTO_TEST_CASE(static_ibitstream_msb) {
  // Testing 1 segment lets us catch buffer overflows.
  // Testing 10 gives us more data and tests the segment boundaries.
  boost::array<std::size_t, 2> segments = {1, 10};
  for(int i = 0; i < segments.size(); ++i) {
    check_static_istream_msb<1>(segments[i]);
    check_static_istream_msb<2>(segments[i]);
    check_static_istream_msb<3>(segments[i]);
    check_static_istream_msb<4>(segments[i]);
    check_static_istream_msb<5>(segments[i]);
    check_static_istream_msb<6>(segments[i]);
    check_static_istream_msb<7>(segments[i]);
  }
}

BOOST_AUTO_TEST_CASE(static_obitstream_lsb) {
  // Testing 1 segment lets us catch buffer overflows.
  // Testing 10 gives us more data and tests the segment boundaries.
  boost::array<std::size_t, 2> segments = {1, 10};
  for(int i = 0; i < segments.size(); ++i) {
    check_static_ostream_lsb<1>(segments[i]);
    check_static_ostream_lsb<2>(segments[i]);
    check_static_ostream_lsb<3>(segments[i]);
    check_static_ostream_lsb<4>(segments[i]);
    check_static_ostream_lsb<5>(segments[i]);
    check_static_ostream_lsb<6>(segments[i]);
    check_static_ostream_lsb<7>(segments[i]);
  }
}

BOOST_AUTO_TEST_CASE(static_obitstream_msb) {
  // Testing 1 segment lets us catch buffer overflows.
  // Testing 10 gives us more data and tests the segment boundaries.
  boost::array<std::size_t, 2> segments = {1, 10};
  for(int i = 0; i < segments.size(); ++i) {
    check_static_ostream_msb<1>(segments[i]);
    check_static_ostream_msb<2>(segments[i]);
    check_static_ostream_msb<3>(segments[i]);
    check_static_ostream_msb<4>(segments[i]);
    check_static_ostream_msb<5>(segments[i]);
    check_static_ostream_msb<6>(segments[i]);
    check_static_ostream_msb<7>(segments[i]);
  }
}

BOOST_AUTO_TEST_CASE(static_iobitstream_lsb) {
  // Testing 1 segment lets us catch buffer overflows.
  // Testing 10 gives us more data and tests the segment boundaries.
  boost::array<std::size_t, 2> segments = {1, 10};
  for(int i = 0; i < segments.size(); ++i) {
    check_static_iostream_lsb<1>(segments[i]);
    check_static_iostream_lsb<2>(segments[i]);
    check_static_iostream_lsb<3>(segments[i]);
    check_static_iostream_lsb<4>(segments[i]);
    check_static_iostream_lsb<5>(segments[i]);
    check_static_iostream_lsb<6>(segments[i]);
    check_static_iostream_lsb<7>(segments[i]);
  }
}

BOOST_AUTO_TEST_CASE(static_iobitstream_msb) {
  // Testing 1 segment lets us catch buffer overflows.
  // Testing 10 gives us more data and tests the segment boundaries.
  boost::array<std::size_t, 2> segments = {1, 10};
  for(int i = 0; i < segments.size(); ++i) {
    check_static_iostream_msb<1>(segments[i]);
    check_static_iostream_msb<2>(segments[i]);
    check_static_iostream_msb<3>(segments[i]);
    check_static_iostream_msb<4>(segments[i]);
    check_static_iostream_msb<5>(segments[i]);
    check_static_iostream_msb<6>(segments[i]);
    check_static_iostream_msb<7>(segments[i]);
  }
}
