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

#include <boost/array.hpp>
#include <boost/radix/bitmask.hpp>
#include <boost/radix/detail/bits.hpp>
#include <boost/type_traits/integral_constant.hpp>

namespace boost { namespace radix {

template <std::size_t Bits>
class static_ibitstream_msb {
 private:
  template <typename UnpackedSegment>
  static void unpack(
      bits_type const* packed,
      UnpackedSegment& out,
      boost::integral_constant<std::size_t, 1>) {
    // [1, 1, 1, 1]
    for(int i = 0; i < 8; ++i) {
      out[i] = (packed[0] >> (7 - i)) & mask<1>::value;
    }
  }

  template <typename RandomAccessInputIterator, typename UnpackedSegment>
  static void unpack(
      RandomAccessInputIterator packed,
      UnpackedSegment& out,
      boost::integral_constant<std::size_t, 2>) {
    // [2, 2, 2, 2]
    out[0] = (packed[0] >> 6) & mask<2>::value;
    out[1] = (packed[0] >> 4) & mask<2>::value;
    out[2] = (packed[0] >> 2) & mask<2>::value;
    out[3] = (packed[0] >> 0) & mask<2>::value;
  };

  template <typename RandomAccessInputIterator, typename UnpackedSegment>
  static void unpack(
      RandomAccessInputIterator packed,
      UnpackedSegment& out,
      boost::integral_constant<std::size_t, 3>) {
    // [3, 3, 2], [1, 3, 3, 1], [2, 3, 3]
    out[0] = (packed[0] >> 5) & mask<3>::value;
    out[1] = (packed[0] >> 2) & mask<3>::value;
    out[2] = ((packed[0] << 1) & mask_shift<2, 1>::value) +
             ((packed[1] >> 7) & mask<1>::value);
    out[3] = (packed[1] >> 4) & mask<3>::value;
    out[4] = (packed[1] >> 1) & mask<3>::value;
    out[5] = ((packed[1] << 2) & mask_shift<1, 2>::value) +
             ((packed[2] >> 6) & mask<2>::value);
    out[6] = (packed[2] >> 3) & mask<3>::value;
    out[7] = (packed[2] >> 0) & mask<3>::value;
  }

  template <typename RandomAccessInputIterator, typename UnpackedSegment>
  static void unpack(
      RandomAccessInputIterator packed,
      UnpackedSegment& out,
      boost::integral_constant<std::size_t, 4>) {
    // [4, 4]
    out[0] = (packed[0] >> 4) & mask<4>::value;
    out[1] = (packed[0] >> 0) & mask<4>::value;
  }

  template <typename RandomAccessInputIterator, typename UnpackedSegment>
  static void unpack(
      RandomAccessInputIterator packed,
      UnpackedSegment& out,
      boost::integral_constant<std::size_t, 5>) {
    // [5, 3], [2, 5, 1], [4, 4], [1, 5, 2], [3, 5]
    out[0] = (packed[0] >> 3) & mask<5>::value;
    out[1] = ((packed[0] << 2) & mask_shift<3, 2>::value) +
             ((packed[1] >> 6) & mask<2>::value);
    out[2] = (packed[1] >> 1) & mask<5>::value;
    out[3] = ((packed[1] << 4) & mask_shift<1, 4>::value) +
             ((packed[2] >> 4) & mask<4>::value);
    out[4] = ((packed[2] << 1) & mask_shift<4, 1>::value) +
             ((packed[3] >> 7) & mask<1>::value);
    out[5] = (packed[3] >> 2) & mask<5>::value;
    out[6] = ((packed[3] << 3) & mask_shift<2, 3>::value) +
             ((packed[4] >> 5) & mask<3>::value);
    out[7] = (packed[4] >> 0) & mask<5>::value;
  }

  template <typename RandomAccessInputIterator, typename UnpackedSegment>
  static void unpack(
      RandomAccessInputIterator packed,
      UnpackedSegment& out,
      boost::integral_constant<std::size_t, 6>) {
    // [6, 2], [4, 4], [2, 6]
    out[0] = (packed[0] >> 2);
    out[1] = ((packed[0] << 4) & mask_shift<2, 4>::value) + ((packed[1] >> 4));
    out[2] = ((packed[1] << 2) & mask_shift<4, 2>::value) + ((packed[2] >> 6));
    out[3] = (packed[2] >> 0) & mask<6>::value;
  }

  template <typename RandomAccessInputIterator, typename UnpackedSegment>
  static void unpack(
      RandomAccessInputIterator packed,
      UnpackedSegment& out,
      boost::integral_constant<std::size_t, 7>) {
    // [7, 1], [6, 2], [5, 3], [4, 4], [3, 5], [2, 6], [1, 7]
    out[0] = (packed[0] >> 1) & mask<7>::value;
    out[1] = ((packed[0] << 6) & mask_shift<1, 6>::value) +
             ((packed[1] >> 2) & mask<6>::value);
    out[2] = ((packed[1] << 5) & mask_shift<2, 5>::value) +
             ((packed[2] >> 3) & mask<5>::value);
    out[3] = ((packed[2] << 4) & mask_shift<3, 4>::value) +
             ((packed[3] >> 4) & mask<4>::value);
    out[4] = ((packed[3] << 3) & mask_shift<4, 3>::value) +
             ((packed[4] >> 5) & mask<3>::value);
    out[5] = ((packed[4] << 2) & mask_shift<5, 2>::value) +
             ((packed[5] >> 6) & mask<2>::value);
    out[6] = ((packed[5] << 1) & mask_shift<6, 1>::value) +
             ((packed[6] >> 7) & mask<1>::value);
    out[7] = (packed[6] >> 0) & mask<7>::value;
  }

 public:
  template <typename RandomAccessInputIterator, typename UnpackedSegment>
  void operator()(
      RandomAccessInputIterator packed, UnpackedSegment& out) const {
    unpack(packed, out, boost::integral_constant<std::size_t, Bits>());
  }
};

}} // namespace boost::radix

#endif // BOOST_RADIX_STATICIBITSTREAMMSB_HPP
