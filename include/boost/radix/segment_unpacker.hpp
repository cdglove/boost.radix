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

#include <boost/integer/integer_mask.hpp>
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
        bits_type bits = packed[0];
        for(int i = 0; i < 8; ++i)
        {
            unpacked[i] = (bits >> 7 - i) & low_bits_mask_t<1>::sig_bits_fast;
        }
    };
};

template <std::size_t BitSize>
struct big_endian_segment_unpacker_impl<2>
{
    template <typename PackedSegment, typename UnpackedSegment>
    void operator()(PackedSegment const& packed, UnpackedSegment& unpacked)
    {
        bits_type bits = packed[0];
        unpacked[0]    = (bits >> 6) & low_bits_mask_t<2>::sig_bits_fast;
        unpacked[1]    = (bits >> 4) & low_bits_mask_t<2>::sig_bits_fast;
        unpacked[2]    = (bits >> 2) & low_bits_mask_t<2>::sig_bits_fast;
        unpacked[3]    = (bits >> 0) & low_bits_mask_t<2>::sig_bits_fast;
    };
};

template <std::size_t BitSize>
struct big_endian_segment_unpacker_impl<3>
{
    template <typename PackedSegment, typename UnpackedSegment>
    void operator()(PackedSegment const& packed, UnpackedSegment& unpacked)
    {
        bits_type bits = packed[0];
        unpacked[0]    = (packed[0] >> 5) & low_bits_mask_t<3>::sig_bits_fast;
        unpacked[1]    = (packed[0] >> 2) & low_bits_mask_t<3>::sig_bits_fast;
        unpacked[2] =
            (((packed[0] >> 0) & low_bits_mask_t<2>::sig_bits_fast) |
             ((packed[1] << 2) & low_bits_mask_t<3>::sig_bits_fast));

        unpacked[3] = (packed[1] >> 4) & low_bits_mask_t<3>::sig_bits_fast;
        unpacked[4] = (packed[1] >> 1) & low_bits_mask_t<3>::sig_bits_fast;
        unpacked[5] = ((packed[1] >> 0) & low_bits_mask_t<3>::sig_bits_fast) | 
                      ((packed[1] >> 7) & low_bits_mask_t<1>::sig_bits_fast));
    };
};

template <std::size_t BitSize>
struct big_endian_segment_unpacker_impl;

template <std::size_t BitSize>
struct big_endian_segment_unpacker_impl;

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
struct big_endian_segment_unpacker_impl;

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