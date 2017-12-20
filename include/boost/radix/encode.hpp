//
// boost/radix/encode.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_ENCODE_HPP
#define BOOST_RADIX_ENCODE_HPP

#include <boost/radix/common.hpp>

#include <boost/radix/pad.hpp>
#include <boost/radix/segment.hpp>
#include <boost/radix/static_ibitstream_msb.hpp>

#include <boost/array.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#    pragma once
#endif

// -----------------------------------------------------------------------------
//
namespace boost { namespace radix {

// -----------------------------------------------------------------------------
//
namespace adl {

template <typename Codec>
std::size_t get_encoded_size(std::size_t source_size, Codec const& codec)
{
    // By default, size is an integer multiple of the output
    // segment size.
    return unpacked_segment_size<Codec>::value *
           (source_size + (packed_segment_size<Codec>::value - 1) /
                              packed_segment_size<Codec>::value);
}

template <typename Codec>
static_ibitstream_msb<required_bits<Codec>::value>
get_segment_unpacker(Codec const& codec)
{
    return static_ibitstream_msb<required_bits<Codec>::value>();
}

} // namespace adl

// -----------------------------------------------------------------------------
//
namespace detail {

template <typename Codec>
struct bits_to_char_mapper
{
    bits_to_char_mapper(Codec const& codec)
        : codec_(codec)
    {}

    char_type operator()(bits_type bits) const
    {
        return codec_.char_from_bits(bits);
    }

    Codec const& codec_;
};

template <
    typename Iterator,
    typename EndIterator,
    typename Codec,
    typename PackedSegment>
void get_packed_segment(
    Iterator& first,
    EndIterator last,
    Codec const& codec,
    PackedSegment& packed)
{
    PackedSegment::iterator pbegin = packed.begin();
    PackedSegment::iterator pend   = packed.end();

    while(first != last && pbegin != pend)
    {
        *pbegin++ = *first++;
    }

    packed.resize(std::distance(packed.begin(), pbegin));
    std::fill(pbegin, pend, 0);
}

template <typename Codec, typename PackedSegment, typename UnpackedSegment>
void unpack_segment(
    Codec const& codec, PackedSegment const& packed, UnpackedSegment& unpacked)
{
    using boost::radix::adl::unpack_segment;
    unpack_segment(codec, packed, unpacked);
    return unpacked;
}

template <typename Codec, typename PackedBuffer, typename UnpackedBuffer>
void maybe_pad_segment(
    Codec const& codec,
    PackedBuffer const& packed,
    UnpackedBuffer& unpacked,
    boost::true_type)
{
    std::size_t bits_written = packed.size() * 8;
    std::size_t bytes_written =
        (bits_written + (required_bits<Codec>::value - 1)) /
        required_bits<Codec>::value;

    while(bytes_written < unpacked.size())
        unpacked[bytes_written++] = codec.get_pad_bits();
}

template <typename Codec, typename PackedBuffer, typename UnpackedBuffer>
void maybe_pad_segment(
    Codec const& codec,
    PackedBuffer const& packed,
    UnpackedBuffer& unpacked,
    boost::false_type)
{}

} // namespace detail

// -----------------------------------------------------------------------------
//
template <typename Codec>
std::size_t encoded_size(std::size_t source_size, Codec const& codec)
{
    using boost::radix::adl::get_encoded_size;
    return get_encoded_size(source_size, codec);
}

// -----------------------------------------------------------------------------
//
template <
    typename InputIterator,
    typename InputEndIterator,
    typename OutputIterator,
    typename Codec,
    typename SegmentUnpacker>
void encode(
    InputIterator first,
    InputEndIterator last,
    OutputIterator out,
    Codec const& codec,
    SegmentUnpacker unpacker)
{
    if(first == last)
        return;

    while(true)
    {
        detail::segment_buffer<bits_type, packed_segment_size<Codec>::value>
            packed_segment;

        ::boost::radix::detail::get_packed_segment(
            first, last, codec, packed_segment);

        boost::array<char_type, unpacked_segment_size<Codec>::value>
            unpacked_segment;

        unpacker(packed_segment, unpacked_segment);

        if(first == last)
        {
            ::boost::radix::detail::maybe_pad_segment(
                codec, packed_segment, unpacked_segment,
                requires_pad<Codec>::type());

            out = std::transform(
                unpacked_segment.begin(), unpacked_segment.end(), out,
                ::boost::radix::detail::bits_to_char_mapper<Codec>(codec));

            break;
        }

        out = std::transform(
            unpacked_segment.begin(), unpacked_segment.end(), out,
            ::boost::radix::detail::bits_to_char_mapper<Codec>(codec));
    }
}

// -----------------------------------------------------------------------------
//
template <
    typename InputIterator,
    typename InputEndIterator,
    typename OutputIterator,
    typename Codec>
void encode(
    InputIterator first,
    InputEndIterator last,
    OutputIterator out,
    Codec const& codec)
{
    using boost::radix::adl::get_segment_unpacker;
    encode(first, last, out, codec, get_segment_unpacker(codec));
}

}} // namespace boost::radix

#endif // BOOST_RADIX_ENCODE_HPP
