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
#include <boost/radix/detail/bits.hpp>

namespace boost { namespace radix {

template <std::size_t Bits>
class static_ibitstream_lsb
{
private:
    BOOST_STATIC_CONSTANT(
        std::size_t, SegmentSize = detail::bits_lcm<Bits>::type::value / Bits);

    template <std::size_t Offset, std::size_t ReadsRemaining>
    struct read_op
    {
    private:
        struct split_read
        {};

        struct single_read
        {};

        template <typename PackedSegment, typename UnpackedSegment>
        static void this_read(
            PackedSegment const& packed, UnpackedSegment& unpacked, split_read)
        {
            bits_type first  = packed[Offset / 8] >> Offset % 8;
            bits_type second = packed[(Offset + Bits) / 8]
                               << (8 - (Offset % 8));

            unpacked[SegmentSize - ReadsRemaining] =
                (first | second) & mask<Bits>::value;
        }

        template <typename PackedSegment, typename UnpackedSegment>
        static void this_read(
            PackedSegment const& packed, UnpackedSegment& unpacked, single_read)
        {
            unpacked[SegmentSize - ReadsRemaining] =
                (packed[Offset / 8] >> Offset % 8) & mask<Bits>::value;
        }

        struct do_read
        {};

        struct nop_read
        {};

        template <typename PackedSegment, typename UnpackedSegment>
        static void next_read(
            PackedSegment const& packed, UnpackedSegment& unpacked, do_read)
        {
            read_op<Offset + Bits, ReadsRemaining - 1>::read(packed, unpacked);
        }

        template <typename PackedSegment, typename UnpackedSegment>
        static void next_read(
            PackedSegment const& packed, UnpackedSegment& unpacked, nop_read)
        {}

    public:
        template <typename PackedSegment, typename UnpackedSegment>
        static void read(PackedSegment const& packed, UnpackedSegment& unpacked)
        {
            typedef typename boost::conditional<
                Offset / 8 == (Offset + Bits) / 8 || ReadsRemaining == 1,
                single_read, split_read>::type this_read_type;

            this_read(packed, unpacked, this_read_type());

            typedef typename boost::conditional<
                ReadsRemaining - 1 == 0, nop_read, do_read>::type
                next_read_type;

            next_read(packed, unpacked, next_read_type());
        };
    };

public:
    template <typename PackedSegment, typename UnpackedSegment>
    void
    operator()(PackedSegment const& packed, UnpackedSegment& unpacked) const
    {
        read_op<0, SegmentSize>::read(packed, unpacked);
    }
};

}} // namespace boost::radix

#endif // BOOST_RADIX_STATICIBITSTREAMLSB_HPP
