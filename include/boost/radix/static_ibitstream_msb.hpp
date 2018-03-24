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

#include <boost/cstdint.hpp>

#include <boost/radix/bitmask.hpp>
#include <boost/radix/detail/bits.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif

namespace boost { namespace radix {

template <std::size_t Bits>
class static_ibitstream_msb {
 public:
  template <typename RandomAccessIterator, typename UnpackedSegment>
  void operator()(RandomAccessIterator bytes, UnpackedSegment& u) const {
    using namespace bits;
    std::size_t const unpacked_size = to_unpacked_segment_size<Bits>::value;
    std::size_t const packed_size   = to_packed_segment_size<Bits>::value;

    BOOST_STATIC_ASSERT(packed_size <= sizeof(boost::uint64_t));

    boost::uint64_t packed = 0;
    for(std::size_t i = 0; i < packed_size; ++i) {
      packed <<= 8;
      packed |= *bytes++;
    }

    for(int i = static_cast<int>(unpacked_size) - 1; i >= 0; --i) {
      u[i] = static_cast<char_type>(packed & mask<Bits>::value);
      packed >>= Bits;
    }
  }
};

}} // namespace boost::radix

#endif // BOOST_RADIX_STATICIBITSTREAMMSB_HPP
