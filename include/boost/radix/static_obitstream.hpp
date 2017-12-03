//
// boost/radix/static_ibitstream.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_STATICOBITSTREAM_HPP
#define BOOST_RADIX_STATICOBITSTREAM_HPP

#include <boost/radix/common.hpp>

#include <boost/radix/bitmask.hpp>

namespace boost { namespace radix {

template <std::size_t Bits, std::size_t SegmentSize>
class static_obitstream
{
private:
    template <std::size_t Offset, std::size_t ReadsRemaining>
    struct write_op;

    template <std::size_t Offset>
    struct write_op<Offset, 0>
    {
        template <typename UnpackedSegment, typename PackedSegment>
        static void
        write(UnpackedSegment const& unpacked, PackedSegment& packed){
            // nop
        };
    };

    template <std::size_t Offset, std::size_t ReadsRemaining>
    struct write_op
    {
    private:
        struct split_write
        {};

        struct single_write
        {};

        template <typename UnpackedSegment, typename PackedSegment>
        static void do_write(
            UnpackedSegment const& unpacked, PackedSegment& packed, split_write)
        {
            bits_type first  = packed[Offset / 8] >> Offset % 8;
            bits_type second = packed[(Offset + Bits) / 8]
                               << (8 - (Offset % 8));
            unpacked[SegmentSize - ReadsRemaining] =
                (first | second) & mask<Bits>::value;
        }

        template <typename UnpackedSegment, typename PackedSegment>
        static void do_write(
            UnpackedSegment const& unpacked,
            PackedSegment& packed,
            single_write)
        {
            unpacked[SegmentSize - ReadsRemaining] =
                (packed[Offset / 8] >> Offset % 8) & mask<Bits>::value;
        }

    public:
        template <typename UnpackedSegment, typename PackedSegment>
        static void
        write(UnpackedSegment const& unpacked, PackedSegment& packed)
        {
            typedef typename boost::conditional<
                Offset / 8 == (Offset + Bits) / 8, single_write,
                split_write>::type write_type;

            do_write(packed, unpacked, write_type());
            write_op<Offset + Bits, ReadsRemaining - 1>::write(
                packed, unpacked);
        };
    };

public:
    template <typename UnpackedSegment, typename PackedSegment>
    static void write(UnpackedSegment const& unpacked, PackedSegment& packed)
    {
        write_op<Bits, 0, SegmentSize>::read(packed, unpacked);
    }
};

}} // namespace boost::radix

#endif // BOOST_RADIX_STATICOBITSTREAM_HPP
