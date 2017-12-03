//
// boost/radix/static_ibitstream.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_STATICIBITSTREAM_HPP
#define BOOST_RADIX_STATICIBITSTREAM_HPP

#include <boost/radix/common.hpp>

namespace boost { namespace radix {

template <std::size_t Bits, std::size_t SegmentSize>
class static_ibitstream
{
private:
    template <std::size_t Offset, std::size_t ReadsRemaining>
    struct read_op;

    template <std::size_t Offset>
    struct read_op<Offset, 0>
    {
        template <typename PackedSegment, typename UnpackedSegment>
        static void
        read(PackedSegment const& packed, UnpackedSegment& unpacked){
            // nop.
        };
    };

    template <std::size_t Offset, std::size_t ReadsRemaining>
    struct read_op
    {
    private:

        struct split_read {};
        struct single_read {};

        template <typename PackedSegment, typename UnpackedSegment>
        static void do_read(
            PackedSegment const& packed, UnpackedSegment& unpacked, split_read)
        {
            bits_type first  = packed[Offset / 8] >> Offset % 8;
            bits_type second = packed[(Offset + Bits) / 8]
                               << (8 - (Offset % 8));

            unpacked[SegmentSize - ReadsRemaining] =
                (first | second) & mask<Bits>::value;
        }

        template <typename PackedSegment, typename UnpackedSegment>
        static void do_read(
            PackedSegment const& packed, UnpackedSegment& unpacked, single_read)
        {
            unpacked[SegmentSize - ReadsRemaining] =
                (packed[Offset / 8] >> Offset % 8) & mask<Bits>::value;
        }
    public:
        template <typename PackedSegment, typename UnpackedSegment>
        static void read(PackedSegment const& packed, UnpackedSegment& unpacked)
        {
            typedef typename boost::conditional<
                Offset / 8 == (Offset + Bits) / 8, single_read,
                split_read>::type read_type;

            do_read(packed, unpacked, read_type());
            read_op<Offset + Bits, ReadsRemaining - 1>::read(
                packed, unpacked);
        };
    };

public:
    template <typename PackedSegment, typename UnpackedSegment>
    static void unpack(PackedSegment const& packed, UnpackedSegment& unpacked)
    {
        read_op<0, SegmentSize>::read(packed, unpacked);
    }
};

}} // namespace boost::radix

#endif // BOOST_RADIX_STATICIBITSTREAM_HPP