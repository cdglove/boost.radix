//
// boost/radix/static_obitstream_msb.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_STATICOBITSTREAMMSB_HPP
#define BOOST_RADIX_STATICOBITSTREAMMSB_HPP

#include <boost/radix/common.hpp>

#include <boost/cstdint.hpp>

#include <boost/radix/bitmask.hpp>
#include <boost/radix/detail/bits.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif

namespace boost { namespace radix {

template <std::size_t Bits>
class static_obitstream_msb {
 public:
  template <typename UnpackedSegment, typename OutputIterator>
  OutputIterator operator()(
      UnpackedSegment const& unpacked, OutputIterator out) const {
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
};
}} // namespace boost::radix

#endif // BOOST_RADIX_STATICOBITSTREAMMSB_HPP
