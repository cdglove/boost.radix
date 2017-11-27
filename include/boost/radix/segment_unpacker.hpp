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

#include <boost/radix/common.hpp>

namespace boost { namespace radix {

namespace detail {

    template<std::size_t BitSize, std::size_t TotalBits, std::size_t Offset, bool Done>
    struct big_endian_segment_unpacker_impl;

    template<std::size_t BitSize, std::size_t TotalBits, std::size_t Offset>
    struct big_endian_segment_unpacker_impl<BitSize, TotalBits, Offset, true>
    {
        template<typename PackedSegment, typename UnpackedSegment>
        void operator()(UnpackedSegment& unpacked, PackedSegment const& packed)
        {}
    };

    template<std::size_t BitSize, std::size_t TotalBits, std::size_t Offset>
    struct big_endian_segment_unpacker_impl<BitSize, TotalBits, Offset, false>
    {
        template<typename PackedSegment, typename UnpackedSegment>
        void operator()(UnpackedSegment& unpacked, PackedSegment const& packed)
        {
            std::size_t byte_index = Offset / 8;
            //std::size_t 
            bits_type bits = packed[byte_index];
            big_endian_segment_unpacker_impl<BitSize, TotalBits, Offset + BitSize, Offset + BitSize < TotalBits>()(unpacked, packed);
        }
    };
}

template<std::size_t BitSize, std::size_t UnpackedSegmentSize>
struct big_endian_segment_unpacker
{
    template<typename PackedSegment>
    boost::array<bits_type, UnpackedSegmentSize> operator()(PackedSegment const& packed)
    {
        boost::array<bits_type, UnpackedSegmentSize> unpacked;
        detail::big_endian_segment_unpacker_impl<BitSize, UnpackedSegmentSize * 8, 0, false>()(packed, unpacked);
        return unpacked;
    }
};


}}

#endif // BOOST_RADIX_SEGMENTUNPACKER_HPP