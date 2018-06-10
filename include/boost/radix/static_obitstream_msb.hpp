//
// boost/radix/static_obitstream_msb.hpp
//
// Copyright (c) Chris Glover, 2017-2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_STATICOBITSTREAMMSB_HPP
#define BOOST_RADIX_STATICOBITSTREAMMSB_HPP

#include <boost/radix/common.hpp>

#include <boost/radix/bitmask.hpp>
#include <boost/radix/detail/bits.hpp>
#include <boost/type_traits/integral_constant.hpp>

#include <boost/cstdint.hpp>

namespace boost { namespace radix {
namespace detail {

template <typename UnpackedSegment, typename OutputIterator>
OutputIterator pack(
    UnpackedSegment const& unpacked,
    OutputIterator out,
    boost::integral_constant<std::size_t, 1>) {
  // [1, 1, 1, 1]
  bits_type packed_bits = 0;
  for(int i = 0, j = 7; i < 8; ++i, --j) {
    packed_bits |= unpacked[i] << j;
  }

  *out++ = packed_bits;
  return out;
}

template <typename UnpackedSegment, typename OutputIterator>
OutputIterator pack(
    UnpackedSegment const& unpacked,
    OutputIterator out,
    boost::integral_constant<std::size_t, 2>) {
  // [2, 2, 2, 2]
  bits_type packed_bits = 0;
  packed_bits |= unpacked[0] << 6;
  packed_bits |= unpacked[1] << 4;
  packed_bits |= unpacked[2] << 2;
  packed_bits |= unpacked[3] << 0;
  *out++ = packed_bits;
  return out;
};

template <typename UnpackedSegment, typename OutputIterator>
OutputIterator pack(
    UnpackedSegment const& unpacked,
    OutputIterator out,
    boost::integral_constant<std::size_t, 3>) {
  // [3, 3, 2], [1, 3, 3, 1], [2, 3, 3]
  *out++ = (unpacked[0] << 5) | (unpacked[1] << 2) | (unpacked[2] >> 1);
  *out++ = (unpacked[2] << 7) | (unpacked[3] << 4) | (unpacked[4] << 1) |
           (unpacked[5] >> 2);
  *out++ = (unpacked[5] << 6) | (unpacked[6] << 3) | (unpacked[7] >> 0);
  return out;
}

template <typename UnpackedSegment, typename OutputIterator>
OutputIterator pack(
    UnpackedSegment const& unpacked,
    OutputIterator out,
    boost::integral_constant<std::size_t, 4>) {
  // [4, 4]
  *out++ = (unpacked[0] << 4) | (unpacked[1] >> 0);
  return out;
}

template <typename UnpackedSegment, typename OutputIterator>
OutputIterator pack(
    UnpackedSegment const& unpacked,
    OutputIterator out,
    boost::integral_constant<std::size_t, 5>) {
  // [5, 3], [2, 5, 1], [4, 4], [1, 5, 2], [3, 5]
  *out++ = (unpacked[0] << 3) | (unpacked[1] >> 2);
  *out++ = (unpacked[1] << 6) | (unpacked[2] << 1) | (unpacked[3] >> 4);
  *out++ = (unpacked[3] << 4) | (unpacked[4] >> 1);
  *out++ = (unpacked[4] << 7) | (unpacked[5] << 2) | (unpacked[6] >> 3);
  *out++ = (unpacked[6] << 5) | (unpacked[7] >> 0);
  return out;
}

template <typename UnpackedSegment, typename OutputIterator>
OutputIterator pack(
    UnpackedSegment const& unpacked,
    OutputIterator out,
    boost::integral_constant<std::size_t, 6>) {
  // [6, 2], [4, 4], [2, 6]
  *out++ = (unpacked[0] << 2) | (unpacked[1] >> 4);
  *out++ = (unpacked[1] << 4) | (unpacked[2] >> 2);
  *out++ = (unpacked[2] << 6) | (unpacked[3] >> 0);
  return out;
}

template <typename UnpackedSegment, typename OutputIterator>
OutputIterator pack(
    UnpackedSegment const& unpacked,
    OutputIterator out,
    boost::integral_constant<std::size_t, 7>) {
  // [7, 1], [6, 2], [5, 3], [4, 4], [3, 5], [2, 6], [1, 7]
  *out++ = (unpacked[0] << 1) | (unpacked[1] >> 6);
  *out++ = (unpacked[1] << 2) | (unpacked[2] >> 5);
  *out++ = (unpacked[2] << 3) | (unpacked[3] >> 4);
  *out++ = (unpacked[3] << 4) | (unpacked[4] >> 3);
  *out++ = (unpacked[4] << 5) | (unpacked[5] >> 2);
  *out++ = (unpacked[5] << 6) | (unpacked[6] >> 1);
  *out++ = (unpacked[6] << 7) | (unpacked[7] >> 0);
  return out;
}

template <typename UnpackedSegment, typename OutputIterator, std::size_t Bits>
OutputIterator pack_basic(
    UnpackedSegment const& unpacked,
    OutputIterator out,
    boost::integral_constant<std::size_t, Bits>) {
  using namespace bits;
  std::size_t const unpacked_size = to_unpacked_segment_size<Bits>::value;
  std::size_t const packed_size   = to_packed_segment_size<Bits>::value;

  BOOST_STATIC_ASSERT(packed_size <= sizeof(boost::uint64_t));

  boost::uint64_t packed = 0;
  for(std::size_t i = 0; i < unpacked_size; ++i) {
    packed <<= Bits;
    packed |= static_cast<bits_type>(unpacked[i] & mask<Bits>::value);
  }

  for(int i = static_cast<int>(packed_size) - 1; i >= 0; --i) {
    *out++ = static_cast<bits_type>(packed >> (i * 8));
  }

  return out;
}

} // namespace detail

template <std::size_t Bits>
class static_obitstream_msb {
 public:
  template <typename UnpackedSegment, typename OutputIterator>
  OutputIterator operator()(
      UnpackedSegment const& unpacked, OutputIterator out) const {
    return detail::pack(
        unpacked, out, boost::integral_constant<std::size_t, Bits>());
  }
};

}} // namespace boost::radix

#endif // BOOST_RADIX_STATICOBITSTREAMMSB_HPP
