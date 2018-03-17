//
// boost/radix/static_ibitstream_msb.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_STATICIBITSTREAMMSB_HPP
#define BOOST_RADIX_STATICIBITSTREAMMSB_HPP

#include <boost/radix/common.hpp>

#include <boost/radix/bitmask.hpp>

namespace boost { namespace radix {

namespace detail {

template <std::size_t BitSize>
struct sequencial_segment_unpacker;

template <>
struct sequencial_segment_unpacker<1> {
  template <typename InputIterator, typename OutputIterator>
  static void unpack(InputIterator& in, OutputIterator& out) {
    // [1, 1, 1, 1]
    bits_type bits = *in++;
    for(int i = 0; i < 8; ++i) {
      *out++ = (bits >> (7 - i)) & mask<1>::value;
    }
  }
};

template <>
struct sequencial_segment_unpacker<2> {
  template <typename InputIterator, typename OutputIterator>
  static void unpack(InputIterator& in, OutputIterator& out) {
    // [2, 2, 2, 2]
    bits_type bits = *in;

    *out++ = (bits >> 6) & mask<2>::value;
    *out++ = (bits >> 4) & mask<2>::value;
    *out++ = (bits >> 2) & mask<2>::value;
    *out++ = (bits >> 0) & mask<2>::value;
  };
};

template <>
struct sequencial_segment_unpacker<3> {
  template <typename InputIterator, typename OutputIterator>
  static void unpack(InputIterator& in, OutputIterator& out) {
    bits_type packed[3];
    for(int i = 0; i < 3; ++i)
        packed[i] = *in++;

    // [3, 3, 2], [1, 3, 3, 1], [2, 3, 3]

    *out++ = (packed[0] >> 5) & mask<3>::value;
    *out++ = (packed[0] >> 2) & mask<3>::value;
    *out++ = ((packed[0] << 1) & mask_shift<2, 1>::value) +
             ((packed[1] >> 7) & mask<1>::value);
    *out++ = (packed[1] >> 4) & mask<3>::value;
    *out++ = (packed[1] >> 1) & mask<3>::value;
    *out++ = ((packed[1] << 2) & mask_shift<1, 2>::value) +
             ((packed[2] >> 6) & mask<2>::value);
    *out++ = (packed[2] >> 3) & mask<3>::value;
    *out++ = (packed[2] >> 0) & mask<3>::value;
  }
};

template <>
struct sequencial_segment_unpacker<4> {
  template <typename InputIterator, typename OutputIterator>
  static void unpack(InputIterator& in, OutputIterator& out) {
    // [4, 4]
    bits_type bits = *in++;

    *out++ = (bits >> 4) & mask<4>::value;
    *out++ = (bits >> 0) & mask<4>::value;
  }
};

template <>
struct sequencial_segment_unpacker<5> {
  template <typename InputIterator, typename OutputIterator>
  static void unpack(InputIterator& in, OutputIterator& out) {
    bits_type packed[5];
    for(int i = 0; i < 5; ++i)
        packed[i] = *in++;

    // [5, 3], [2, 5, 1], [4, 4], [1, 5, 2], [3, 5]
    *out++ = (packed[0] >> 3) & mask<5>::value;
    *out++ = ((packed[0] << 2) & mask_shift<3, 2>::value) +
             ((packed[1] >> 6) & mask<2>::value);
    *out++ = (packed[1] >> 1) & mask<5>::value;
    *out++ = ((packed[1] << 4) & mask_shift<1, 4>::value) +
             ((packed[2] >> 4) & mask<4>::value);
    *out++ = ((packed[2] << 1) & mask_shift<4, 1>::value) +
             ((packed[3] >> 7) & mask<1>::value);
    *out++ = (packed[3] >> 2) & mask<5>::value;
    *out++ = ((packed[3] << 3) & mask_shift<2, 3>::value) +
             ((packed[4] >> 5) & mask<3>::value);
    *out++ = (packed[4] >> 0) & mask<5>::value;
  }
};

template <>
struct sequencial_segment_unpacker<6> {
  template <typename InputIterator, typename OutputIterator>
  static void unpack(InputIterator& in, OutputIterator& out) {
    bits_type packed[4];
    for(int i = 0; i < 4; ++i)
        packed[i] = *in++;

    // [6, 2], [4, 4], [2, 6]
    *out++ = (packed[0] >> 2);
    *out++ = ((packed[0] << 4) & mask_shift<2, 4>::value) + ((packed[1] >> 4));
    *out++ = ((packed[1] << 2) & mask_shift<4, 2>::value) + ((packed[2] >> 6));
    *out++ = (packed[2] >> 0) & mask<6>::value;
  }
};

template <>
struct sequencial_segment_unpacker<7> {
  template <typename InputIterator, typename OutputIterator>
  static void unpack(InputIterator& in, OutputIterator& out) {
    bits_type packed[7];
    for(int i = 0; i < 7; ++i)
        packed[i] = *in++;

    // [7, 1], [6, 2], [5, 3], [4, 4], [3, 5], [2, 6], [1, 7]
    *out++ = (packed[0] >> 1) & mask<7>::value;
    *out++ = ((packed[0] << 6) & mask_shift<1, 6>::value) +
             ((packed[1] >> 2) & mask<6>::value);
    *out++ = ((packed[1] << 5) & mask_shift<2, 5>::value) +
             ((packed[2] >> 3) & mask<5>::value);
    *out++ = ((packed[2] << 4) & mask_shift<3, 4>::value) +
             ((packed[3] >> 4) & mask<4>::value);
    *out++ = ((packed[3] << 3) & mask_shift<4, 3>::value) +
             ((packed[4] >> 5) & mask<3>::value);
    *out++ = ((packed[4] << 2) & mask_shift<5, 2>::value) +
             ((packed[5] >> 6) & mask<2>::value);
    *out++ = ((packed[5] << 1) & mask_shift<6, 1>::value) +
             ((packed[6] >> 7) & mask<1>::value);
    *out++ = (packed[6] >> 0) & mask<7>::value;
  };
};
} // namespace detail

template <std::size_t Bits>
struct static_ibitstream_msb {
  template <typename InputIterator, typename OutputIterator>
  void operator()(InputIterator& in, OutputIterator& out) const {
    detail::sequencial_segment_unpacker<Bits>::unpack(in, out);
  }
};

}} // namespace boost::radix

#endif // BOOST_RADIX_STATICIBITSTREAMMSB_HPP
