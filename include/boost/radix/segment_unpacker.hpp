//
// boost/radix/segment_unpacker.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_SEGMENTUNPACKER_HPP
#define BOOST_RADIX_SEGMENTUNPACKER_HPP

#include <boost/config.hpp>

#include <boost/radix/bitmask.hpp>
#include <boost/radix/common.hpp>

namespace boost { namespace radix {

namespace detail {

template <std::size_t BitSize>
struct big_endian_segment_unpacker_impl;

template <std::size_t BitSize>
struct big_endian_segment_unpacker_impl<1>
{
    template <typename PackedSegment, typename UnpackedSegment>
    void operator()(PackedSegment const& packed, UnpackedSegment& unpacked)
    {
        // [1, 1, 1, 1]
        bits_type bits = packed[0];
        for(int i = 0; i < 8; ++i)
        {
            unpacked[i] = (bits >> 7 - i) & mask<1>::value;
        }
    }
};

template <std::size_t BitSize>
struct big_endian_segment_unpacker_impl<2>
{
    template <typename PackedSegment, typename UnpackedSegment>
    void operator()(PackedSegment const& packed, UnpackedSegment& unpacked)
    {
        // [2, 2, 2, 2]
        bits_type bits = packed[0];
        unpacked[0]    = (bits >> 6) & mask<2>::value;
        unpacked[1]    = (bits >> 4) & mask<2>::value;
        unpacked[2]    = (bits >> 2) & mask<2>::value;
        unpacked[3]    = (bits >> 0) & mask<2>::value;
    };
};

template <std::size_t BitSize>
struct big_endian_segment_unpacker_impl<3>
{
    template <typename PackedSegment, typename UnpackedSegment>
    void operator()(PackedSegment const& packed, UnpackedSegment& unpacked)
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
        unpacked[6] = (packed[3] >> 3) & mask<3>::value;
        unpacked[7] = (packed[3] >> 0) & mask<3>::value;
    }
};

template <std::size_t BitSize>
struct big_endian_segment_unpacker_impl<4>
{
    template <typename PackedSegment, typename UnpackedSegment>
    void operator()(PackedSegment const& packed, UnpackedSegment& unpacked)
    {
        // [4, 4]
        bits_type bits = packed[0];
        unpacked[0]    = (bits >> 4) & mask<4>::value;
        unpacked[1]    = (bits >> 0) & mask<4>::value;
    }
};

template <std::size_t BitSize>
struct big_endian_segment_unpacker_impl<5>
{
    template <typename PackedSegment, typename UnpackedSegment>
    void operator()(PackedSegment const& packed, UnpackedSegment& unpacked)
    {
        // [5, 3], [2, 5, 1], [4, 4], [1, 5, 2], [3, 5]
        unpacked[0] = (packed[0] >> 3) & mask<5>::value;
        unpacked[1] = ((packed[0] >> 2) & mask_shift<3, 2>::value) |
                      ((packed[1] >> 6) & mask<2>::value);
        unpacked[2] = (packed[1] >> 1) & mask<5>::value;
        unpacked[3] = ((packed[1] << 4) & mask_shift<1, 4>::value) |
                      ((packed[2] >> 4) & mask<4>::value);
        unpacked[4] = ((packed[2] << 1) & mask_shift<4, 1>::value) |
                      ((packed[3] >> 7) & mask<1>::value;
        unpacked[5] = (packed[3] >> 2) & mask<5>::value;

        unpacked[6] = ((packed[3] << 3) & mask_shift<2, 3>::value) |
                      ((packed[4] >> 5) & mask<3>::value);

        unpacked[7] = (packed[4] >> 0) & mask<5>::value;
    }
};

template <std::size_t BitSize>
struct big_endian_segment_unpacker_impl<6>
{
    template <typename PackedSegment, typename UnpackedSegment>
    void operator()(PackedSegment const& packed, UnpackedSegment& unpacked)
    {
        unpacked[0] = (packed[0] & 0xfc) >> 2;
        unpacked[1] = ((packed[0] & 0x03) << 4) | ((packed[1] & 0xf0) >> 4);
        unpacked[2] = ((packed[1] & 0x0f) << 2) | ((packed[2] & 0xc0) >> 6);
        unpacked[3] = (packed[2] & 0x3f >> 0;
    };
};

template <std::size_t BitSize>
struct big_endian_segment_unpacker_impl<7>
{
    template <typename PackedSegment, typename UnpackedSegment>
    void operator()(PackedSegment const& packed, UnpackedSegment& unpacked)
    {

    };
};

} // namespace detail

template <std::size_t BitSize, std::size_t UnpackedSegmentSize>
struct big_endian_segment_unpacker
{
    template <typename PackedSegment>
    boost::array<bits_type, UnpackedSegmentSize>
    operator()(PackedSegment const& packed)
    {
        boost::array<bits_type, UnpackedSegmentSize> unpacked;
        detail::big_endian_segment_unpacker_impl<BitSize>()(packed, unpacked);
        return unpacked;
    }
};

}} // namespace boost::radix

#endif // BOOST_RADIX_SEGMENTUNPACKER_HPP