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
#include <boost/radix/static_obitstream.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost { namespace radix {

template <typename Codec>
struct segment_packer_type
{
    typedef static_obitstream<required_bits<Codec>::value> type;
};

// -----------------------------------------------------------------------------
//
namespace adl {

template <typename Codec, typename UnpackedSegment>
typename packed_segment_type<Codec>::type
pack_segment(Codec const& codec, UnpackedSegment const& unpacked)
{
    typename packed_segment_type<Codec>::type packed;
    segment_packer_type<Codec>::type::pack(unpacked, packed);
    return unpacked;
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

template <typename Codec>
std::size_t decoded_size(std::size_t source_size, Codec const& codec)
{
    using boost::radix::adl::get_decoded_size;
    return get_decoded_size(source_size, codec);
}

template <typename InputIterator, typename OutputIterator, typename Codec>
void decode(
    InputIterator first,
    InputIterator last,
    OutputIterator out,
    Codec const& codec)
{
    unsigned char output_buffer[3];
    unsigned char input_buffer[4];

    auto const& alpha = codec;
    int i             = 0;

    while(first != last)
    {
        unsigned char current = *first++;
        if(current == codec.get_pad_char())
            break;

        input_buffer[i++] = current;
        if(i == 4)
        {
            for(i = 0; i < 4; ++i)
                input_buffer[i] = alpha.bits_from_char(input_buffer[i]);

            output_buffer[0] =
                (input_buffer[0] << 2) + ((input_buffer[1] & 0x30) >> 4);
            output_buffer[1] = ((input_buffer[1] & 0xf) << 4) +
                               ((input_buffer[2] & 0x3c) >> 2);
            output_buffer[2] = ((input_buffer[2] & 0x3) << 6) + input_buffer[3];

            for(i = 0; (i < 3); ++i)
                *out++ = output_buffer[i];
            i = 0;
        }
    }

    if(i)
    {
        for(int j = i; j < 4; ++j)
            input_buffer[j] = 0;

        for(int j = 0; j < 4; ++j)
            input_buffer[j] = alpha.bits_from_char(input_buffer[j]);

        output_buffer[0] =
            (input_buffer[0] << 2) + ((input_buffer[1] & 0x30) >> 4);
        output_buffer[1] =
            ((input_buffer[1] & 0xf) << 4) + ((input_buffer[2] & 0x3c) >> 2);
        output_buffer[2] = ((input_buffer[2] & 0x3) << 6) + input_buffer[3];

        for(int j = 0; (j < i - 1); ++j)
            *out++ = output_buffer[j];
    }
}

}} // namespace boost::radix

#endif // BOOST_RADIX_DECODE_HPP