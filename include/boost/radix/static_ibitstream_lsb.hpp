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

#include <boost/array.hpp>
#include <boost/radix/bitmask.hpp>
#include <boost/radix/detail/bits.hpp>
#include <boost/type_traits/conditional.hpp>

namespace boost { namespace radix {

template <std::size_t Bits>
class static_ibitstream_lsb {
 private:
  template <std::size_t Offset, std::size_t ReadsRemaining>
  struct read_op {
   private:
    struct split_read {};
    struct single_read {};
    template <std::size_t SegmentSize, typename OutputIterator>
    static void this_read(
        boost::array<bits_type, SegmentSize> const& packed,
        OutputIterator& out,
        split_read) {
      bits_type first  = packed[Offset / 8] >> Offset % 8;
      bits_type second = packed[(Offset + Bits) / 8] << (8 - (Offset % 8));

      *out++ = (first | second) & mask<Bits>::value;
    }

    template <std::size_t SegmentSize, typename OutputIterator>
    static void this_read(
        boost::array<bits_type, SegmentSize> const& packed,
        OutputIterator& out,
        single_read) {
      *out++ = (packed[Offset / 8] >> Offset % 8) & mask<Bits>::value;
    }

    struct do_read {};
    struct nop_read {};
    template <std::size_t SegmentSize, typename OutputIterator>
    static void next_read(
        boost::array<bits_type, SegmentSize> const& packed,
        OutputIterator& out,
        do_read) {
      read_op<Offset + Bits, ReadsRemaining - 1>::read(packed, out);
    }

    template <std::size_t SegmentSize, typename OutputIterator>
    static void next_read(
        boost::array<bits_type, SegmentSize> const& packed,
        OutputIterator& out,
        nop_read) {
    }

   public:
    template <std::size_t SegmentSize, typename OutputIterator>
    static void read(
        boost::array<bits_type, SegmentSize> const& packed,
        OutputIterator& out) {
      typedef typename boost::conditional<
          Offset / 8 == (Offset + Bits) / 8 || ReadsRemaining == 1, single_read,
          split_read>::type this_read_type;

      this_read(packed, out, this_read_type());

      typedef typename boost::conditional<
          ReadsRemaining - 1 == 0, nop_read, do_read>::type next_read_type;

      next_read(packed, out, next_read_type());
    };
  };

 public:
  template <typename InputIterator, typename OutputIterator>
  void operator()(InputIterator& in, OutputIterator& out) const {
    boost::array<bits_type, bits::to_packed_segment_size<Bits>::value> packed;
    for(int i = 0; i < bits::to_packed_segment_size<Bits>::value; ++i)
      packed[i] = *in++;
    read_op<0, bits::to_unpacked_segment_size<Bits>::value>::read(packed, out);
  }
};

}} // namespace boost::radix

#endif // BOOST_RADIX_STATICIBITSTREAMLSB_HPP
