//
// boost/radix/static_ibitstream_lsb.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_STATICIBITSTREAMLSB_HPP
#define BOOST_RADIX_STATICIBITSTREAMLSB_HPP

#include <boost/radix/common.hpp>

#include <boost/radix/bitmask.hpp>

#include <boost/cstdint.hpp>

namespace boost { namespace radix {

template <std::size_t Bits>
class static_ibitstream_lsb {
 public:
  template <typename RandomAccessIterator, typename UnpackedSegment>
  void operator()(RandomAccessIterator packed, UnpackedSegment& u) const {
    using namespace bits;
    std::size_t const unpacked_size = to_unpacked_segment_size<Bits>::value;
    std::size_t const packed_size   = to_packed_segment_size<Bits>::value;

    BOOST_STATIC_ASSERT(packed_size <= sizeof(boost::uint64_t));

    boost::uint64_t all_bits = 0;
    for(std::size_t i = 0; i < packed_size; ++i) {
      all_bits |= boost::uint64_t(*packed++) << (i * 8);
    }

    for(std::size_t i = 0; i < unpacked_size; ++i) {
      u[i] = bits_type(all_bits & mask<Bits>::value);
      all_bits >>= Bits;
    }
  }
};

}} // namespace boost::radix

#endif // BOOST_RADIX_STATICIBITSTREAMLSB_HPP
