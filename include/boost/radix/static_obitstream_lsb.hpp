//
// boost/radix/static_ibitstream_lsb.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_STATICOBITSTREAMLSB_HPP
#define BOOST_RADIX_STATICOBITSTREAMLSB_HPP

#include <boost/radix/common.hpp>

#include <boost/radix/bitmask.hpp>
#include <boost/radix/codec_traits/segment.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#    pragma once
#endif

namespace boost { namespace radix {

template <std::size_t Bits>
class static_obitstream_lsb
{
private:
    BOOST_STATIC_CONSTANT(
        std::size_t, SegmentSize = detail::bits_lcm<Bits>::type::value / Bits);

    template <std::size_t Offset, std::size_t WritesRemaining>
    struct write_op
    {
    private:
        struct split_write
        {};

        struct single_write
        {};

        template <typename UnpackedSegment, typename PackedSegment>
        static void this_write(
            UnpackedSegment const& unpacked, PackedSegment& packed, split_write)
        {
            bits_type bits = unpacked[SegmentSize - WritesRemaining];
            packed[Offset / 8] |= bits << Offset % 8;

            // We know this is the first write, so we don't need to |
            packed[(Offset + Bits) / 8] =
                (bits & mask<Bits>::value) >> (8 - (Offset % 8));
        }

        template <typename UnpackedSegment, typename PackedSegment>
        static void this_write(
            UnpackedSegment const& unpacked,
            PackedSegment& packed,
            single_write)
        {
            bits_type bits = unpacked[SegmentSize - WritesRemaining];
            packed[Offset / 8] |= (bits & mask<Bits>::value) << Offset % 8;
        }

        struct do_write
        {};

        struct nop_write
        {};

        template <typename UnpackedSegment, typename PackedSegment>
        static void next_write(
            UnpackedSegment const& unpacked, PackedSegment& packed, do_write)
        {
            write_op<Offset + Bits, WritesRemaining - 1>::write(
                unpacked, packed);
        }

        template <typename UnpackedSegment, typename PackedSegment>
        static void
        next_write(UnpackedSegment const&, PackedSegment&, nop_write)
        {}

    public:
        template <typename UnpackedSegment, typename PackedSegment>
        static void
        write(UnpackedSegment const& unpacked, PackedSegment& packed)
        {
            typedef typename boost::conditional<
                (Offset / 8 == (Offset + Bits) / 8) || (WritesRemaining == 1),
                single_write, split_write>::type this_write_type;

            this_write(unpacked, packed, this_write_type());

            typedef typename boost::conditional<
                WritesRemaining - 1 == 0, nop_write, do_write>::type
                next_write_type;

            next_write(unpacked, packed, next_write_type());
        };
    };

public:
    template <typename UnpackedSegment, typename PackedSegment>
    void
    operator()(UnpackedSegment const& unpacked, PackedSegment& packed) const
    {
        // We need to zero the first bytes, the rest will be taken care
        // of with split write.
        packed[0] = 0;
        write_op<0, SegmentSize>::write(unpacked, packed);
    }
};

}} // namespace boost::radix

#endif // BOOST_RADIX_STATICOBITSTREAMLSB_HPP
