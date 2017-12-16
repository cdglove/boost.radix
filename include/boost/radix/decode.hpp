//
// boost/radix/decode.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_DECODE_HPP
#define BOOST_RADIX_DECODE_HPP

#include <boost/radix/common.hpp>

#include <boost/radix/segment.hpp>
#include <boost/radix/static_obitstream_msb.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#    pragma once
#endif

namespace boost { namespace radix {

// -----------------------------------------------------------------------------
//
namespace adl {

template <typename Codec>
static_obitstream_msb<required_bits<Codec>::value>
get_segment_packer(Codec const& codec)
{
    return static_obitstream_msb<required_bits<Codec>::value>();
}

template <typename Codec>
std::size_t get_decoded_size(std::size_t source_size, Codec const& codec)
{
    // By default, size is an integer multiple of the output
    // segment size.
    return packed_segment_size<Codec>::value *
           (source_size + (unpacked_segment_size<Codec>::value - 1) /
                              unpacked_segment_size<Codec>::value);
}

} // namespace adl

namespace detail {

template <
    typename Iterator,
    typename EndIterator,
    typename Codec,
    typename UnpackedSegment>
void get_unpacked_segment(
    Iterator& first,
    EndIterator last,
    Codec const& codec,
    UnpackedSegment& buffer)
{
    UnpackedSegment::iterator bbegin = buffer.begin();
    UnpackedSegment::iterator bend   = buffer.end();

    while(first != last && bbegin != bend)
    {
        *bbegin++ = codec.bits_from_char(*first++);
    }

    if(first == last)
    {
        // cglover: seems maybe bits_from_char should just write 0 when it hits
        // the pad?
        bbegin = std::find(buffer.begin(), bbegin, codec.get_pad_bits());
        buffer.resize(std::distance(buffer.begin(), bbegin));
        std::fill(bbegin, bend, 0);
    }
}

} // namespace detail

// -----------------------------------------------------------------------------
//
template <typename Codec>
std::size_t decoded_size(std::size_t source_size, Codec const& codec)
{
    using boost::radix::adl::get_decoded_size;
    return get_decoded_size(source_size, codec);
}

// -----------------------------------------------------------------------------
//
template <
    typename InputIterator,
    typename InputEndIterator,
    typename OutputIterator,
    typename Codec,
    typename SegmentPacker>
void decode(
    InputIterator first,
    InputEndIterator last,
    OutputIterator out,
    Codec const& codec,
    SegmentPacker packer)
{
    std::size_t total = 0;
    while(true)
    {
        std::size_t dist = std::distance(first, last);
        detail::segment_buffer<bits_type, unpacked_segment_size<Codec>::value>
            unpacked_segment;
        ::boost::radix::detail::get_unpacked_segment(
            first, last, codec, unpacked_segment);

        boost::array<bits_type, packed_segment_size<Codec>::value>
            packed_segment;
        packer(unpacked_segment, packed_segment);

        // If we hit the end, we can't write out everything.
        if(first == last)
        {
            std::size_t bits_to_write =
                packed_segment_size<Codec>::value * required_bits<Codec>::value;
            
            std::size_t bytes_to_write = (bits_to_write + 7) / 8;

            if((unpacked_segment_size<Codec>::value - unpacked_segment.size()) < bytes_to_write)
                bytes_to_write -= (unpacked_segment_size<Codec>::value - unpacked_segment.size());
            else
                bytes_to_write = 1;
         
            std::copy(
                packed_segment.begin(), packed_segment.begin() + bytes_to_write,
                out);

            break;
        }

        total += packed_segment.size();
        out = std::copy(packed_segment.begin(), packed_segment.end(), out);
    }
}

// -----------------------------------------------------------------------------
//
template <
    typename InputIterator,
    typename InputEndIterator,
    typename OutputIterator,
    typename Codec>
void decode(
    InputIterator first,
    InputEndIterator last,
    OutputIterator out,
    Codec const& codec)
{
    using boost::radix::adl::get_segment_packer;
    decode(first, last, out, codec, get_segment_packer(codec));
}

}} // namespace boost::radix

#endif // BOOST_RADIX_DECODE_HPP
