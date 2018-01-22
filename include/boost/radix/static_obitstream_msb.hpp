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

#include <boost/radix/bitmask.hpp>
#include <boost/radix/detail/bits.hpp>

namespace boost { namespace radix {

namespace detail {

template <std::size_t BitSize>
struct sequencial_segment_packer;

template <>
struct sequencial_segment_packer<1>
{
    template <typename UnpackedSegment, typename PackedSegment>
    static void pack(UnpackedSegment const& unpacked, PackedSegment& packed)
    {
        // [1, 1, 1, 1]
        bits_type packed_bits = 0;
        for(int i = 0, j = 7; i < 8; ++i, --j)
        {
            packed_bits |= unpacked[i] << j;
        }

        packed[0] = packed_bits;
    }
};

template <>
struct sequencial_segment_packer<2>
{
    template <typename UnpackedSegment, typename PackedSegment>
    static void pack(UnpackedSegment const& unpacked, PackedSegment& packed)
    {
        // [2, 2, 2, 2]
        bits_type packed_bits = 0;

        packed_bits |= unpacked[0] << 6;
        packed_bits |= unpacked[1] << 4;
        packed_bits |= unpacked[2] << 2;
        packed_bits |= unpacked[3] << 0;

        packed[0] = packed_bits;
    };
};

template <>
struct sequencial_segment_packer<3>
{
    template <typename UnpackedSegment, typename PackedSegment>
    static void pack(UnpackedSegment const& unpacked, PackedSegment& packed)
    {
        // clang-format off
        // [3, 3, 2], [1, 3, 3, 1], [2, 3, 3]
        packed[0] = (unpacked[0] << 5) | (unpacked[1] << 2) | (unpacked[2] >> 1);
        packed[1] = (unpacked[2] << 7) | (unpacked[3] << 4) | (unpacked[4] << 1) | (unpacked[5] >> 2);
        packed[2] = (unpacked[5] << 6) | (unpacked[6] << 3) | (unpacked[7] >> 0);
        // clang-format on
    }
};

template <>
struct sequencial_segment_packer<4>
{
    template <typename UnpackedSegment, typename PackedSegment>
    static void pack(UnpackedSegment const& unpacked, PackedSegment& packed)
    {
        // [4, 4]
        packed[0] = unpacked[0] << 4;
        packed[0] |= unpacked[1] >> 0;
    }
};

template <>
struct sequencial_segment_packer<5>
{
    template <typename UnpackedSegment, typename PackedSegment>
    static void pack(UnpackedSegment const& unpacked, PackedSegment& packed)
    {
        // clang-format off
        // [5, 3], [2, 5, 1], [4, 4], [1, 5, 2], [3, 5]
        packed[0] = (unpacked[0] << 3) | (unpacked[1] >> 2);
        packed[1] = (unpacked[1] << 6) | (unpacked[2] << 1) | (unpacked[3] >> 4);
        packed[2] = (unpacked[3] << 4) | (unpacked[4] >> 1);
        packed[3] = (unpacked[4] << 7) | (unpacked[5] << 2) | (unpacked[6] >> 3);
        packed[4] = (unpacked[6] << 5) | (unpacked[7] >> 0);
        // clang-format on
    }
};

template <>
struct sequencial_segment_packer<6>
{
    template <typename UnpackedSegment, typename PackedSegment>
    static void pack(UnpackedSegment const& unpacked, PackedSegment& packed)
    {
        // [6, 2], [4, 4], [2, 6]
        packed[0] = (unpacked[0] << 2) | (unpacked[1] >> 4);
        packed[1] = (unpacked[1] << 4) | (unpacked[2] >> 2);
        packed[2] = (unpacked[2] << 6) | (unpacked[3] >> 0);
    }
};

template <>
struct sequencial_segment_packer<7>
{
    template <typename UnpackedSegment, typename PackedSegment>
    static void pack(UnpackedSegment const& unpacked, PackedSegment& packed)
    {
        // [7, 1], [6, 2], [5, 3], [4, 4], [3, 5], [2, 6], [1, 7]
        packed[0] = (unpacked[0] << 1) | (unpacked[1] >> 6);
        packed[1] = (unpacked[1] << 2) | (unpacked[2] >> 5);
        packed[2] = (unpacked[2] << 3) | (unpacked[3] >> 4);
        packed[3] = (unpacked[3] << 4) | (unpacked[4] >> 3);
        packed[4] = (unpacked[4] << 5) | (unpacked[5] >> 2);
        packed[5] = (unpacked[5] << 6) | (unpacked[6] >> 1);
        packed[6] = (unpacked[6] << 7) | (unpacked[7] >> 0);
    };
};
} // namespace detail

template <std::size_t Bits>
struct static_obitstream_msb
{
    template <typename UnpackedSegment, typename PackedSegment>
    void
    operator()(UnpackedSegment const& unpacked, PackedSegment&& packed) const
    {
        detail::sequencial_segment_packer<Bits>::pack(unpacked, packed);
    }
};

}} // namespace boost::radix

#endif // BOOST_RADIX_STATICOBITSTREAMMSB_HPP
