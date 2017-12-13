//
// boost/radix/static_ibitstream_msb.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_STATICSEQUENTIALIBITSTREAM_HPP
#define BOOST_RADIX_STATICSEQUENTIALIBITSTREAM_HPP

#include <boost/radix/common.hpp>

#include <boost/radix/bitmask.hpp>

namespace boost { namespace radix {

namespace detail {

template <std::size_t BitSize>
struct sequencial_segment_unpacker;

template <>
struct sequencial_segment_unpacker<1>
{
    template <typename PackedSegment, typename UnpackedSegment>
    static void unpack(PackedSegment const& packed, UnpackedSegment& unpacked)
    {
        // [1, 1, 1, 1]
        bits_type bits = packed[0];
        for(int i = 0; i < 8; ++i)
        {
            unpacked[i] = (bits >> (7 - i)) & mask<1>::value;
        }
    }
};

template <>
struct sequencial_segment_unpacker<2>
{
    template <typename PackedSegment, typename UnpackedSegment>
    static void unpack(PackedSegment const& packed, UnpackedSegment& unpacked)
    {
        // [2, 2, 2, 2]
        bits_type bits = packed[0];

        unpacked[0] = (bits >> 6) & mask<2>::value;
        unpacked[1] = (bits >> 4) & mask<2>::value;
        unpacked[2] = (bits >> 2) & mask<2>::value;
        unpacked[3] = (bits >> 0) & mask<2>::value;
    };
};

template <>
struct sequencial_segment_unpacker<3>
{
    template <typename PackedSegment, typename UnpackedSegment>
    static void unpack(PackedSegment const& packed, UnpackedSegment& unpacked)
    {
        // [3, 3, 2], [1, 3, 3, 1], [2, 3, 3]
        unpacked[0] = (packed[0] >> 5) & mask<3>::value;
        unpacked[1] = (packed[0] >> 2) & mask<3>::value;
        unpacked[2] = ((packed[0] << 1) & mask_shift<2, 1>::value) |
                      ((packed[1] >> 7) & mask<1>::value);
        unpacked[3] = (packed[1] >> 4) & mask<3>::value;
        unpacked[4] = (packed[1] >> 1) & mask<3>::value;
        unpacked[5] = ((packed[1] << 0) & mask_shift<1, 2>::value) |
                      ((packed[2] >> 6) & mask<2>::value);
        unpacked[6] = (packed[2] >> 3) & mask<3>::value;
        unpacked[7] = (packed[2] >> 0) & mask<3>::value;
    }
};

template <>
struct sequencial_segment_unpacker<4>
{
    template <typename PackedSegment, typename UnpackedSegment>
    static void unpack(PackedSegment const& packed, UnpackedSegment& unpacked)
    {
        // [4, 4]
        bits_type bits = packed[0];

        unpacked[0] = (bits >> 4) & mask<4>::value;
        unpacked[1] = (bits >> 0) & mask<4>::value;
    }
};

template <>
struct sequencial_segment_unpacker<5>
{
    template <typename PackedSegment, typename UnpackedSegment>
    static void unpack(PackedSegment const& packed, UnpackedSegment& unpacked)
    {
        // [5, 3], [2, 5, 1], [4, 4], [1, 5, 2], [3, 5]
        unpacked[0] = (packed[0] >> 3) & mask<5>::value;
        unpacked[1] = ((packed[0] << 2) & mask_shift<3, 2>::value) |
                      ((packed[1] >> 6) & mask<2>::value);
        unpacked[2] = (packed[1] >> 1) & mask<5>::value;
        unpacked[3] = ((packed[1] << 4) & mask_shift<1, 4>::value) |
                      ((packed[2] >> 4) & mask<4>::value);
        unpacked[4] = ((packed[2] << 1) & mask_shift<4, 1>::value) |
                      ((packed[3] >> 7) & mask<1>::value);
        unpacked[5] = (packed[3] >> 2) & mask<5>::value;
        unpacked[6] = ((packed[3] << 3) & mask_shift<2, 3>::value) |
                      ((packed[4] >> 5) & mask<3>::value);
        unpacked[7] = (packed[4] >> 0) & mask<5>::value;
    }
};

template <>
struct sequencial_segment_unpacker<6>
{
    template <typename PackedSegment, typename UnpackedSegment>
    static void unpack(PackedSegment const& packed, UnpackedSegment& unpacked)
    {
        // [6, 2], [4, 4], [2, 6]
        unpacked[0] = (packed[0] >> 2) & mask<6>::value;
        unpacked[1] = ((packed[0] << 4) & mask_shift<2, 4>::value) |
                      ((packed[1] >> 4) & mask<4>::value);
        unpacked[2] = ((packed[1] << 2) & mask_shift<4, 2>::value) |
                      ((packed[2] >> 6) & mask<2>::value);
        unpacked[3] = (packed[2] >> 0) & mask<6>::value;
    }
};

template <>
struct sequencial_segment_unpacker<7>
{
    template <typename PackedSegment, typename UnpackedSegment>
    static void unpack(PackedSegment const& packed, UnpackedSegment& unpacked)
    {
        // [7, 1], [6, 2], [5, 3], [4, 4], [3, 5], [2, 6], [1, 7]
        unpacked[0] = (packed[0] >> 1) & mask<7>::value;
        unpacked[1] = ((packed[0] << 6) & mask_shift<1, 6>::value) |
                      ((packed[1] >> 2) & mask<6>::value);
        unpacked[2] = ((packed[1] << 5) & mask_shift<2, 5>::value) |
                      ((packed[2] >> 3) & mask<5>::value);
        unpacked[3] = ((packed[2] << 4) & mask_shift<3, 4>::value) |
                      ((packed[3] >> 4) & mask<4>::value);
        unpacked[4] = ((packed[3] << 3) & mask_shift<4, 3>::value) |
                      ((packed[4] >> 5) & mask<3>::value);
        unpacked[5] = ((packed[4] << 2) & mask_shift<5, 2>::value) |
                      ((packed[5] >> 6) & mask<2>::value);
        unpacked[6] = ((packed[5] << 1) & mask_shift<6, 1>::value) |
                      ((packed[6] >> 6) & mask<1>::value);
        unpacked[7] = (packed[6] >> 0) & mask<7>::value;
    };
};
} // namespace detail

template <std::size_t Bits>
struct static_ibitstream_msb
{
    template <typename PackedSegment, typename UnpackedSegment>
    void operator()(PackedSegment const& packed, UnpackedSegment& unpacked) const
    {
        detail::sequencial_segment_unpacker<Bits>::unpack(packed, unpacked);
    }
};

}} // namespace boost::radix

#endif // BOOST_RADIX_STATICSEQUENTIALIBITSTREAM_HPP
