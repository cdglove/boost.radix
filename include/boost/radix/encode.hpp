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

#include <boost/config.hpp>

#include <boost/radix/common.hpp>
#include <boost/radix/segment_unpacker.hpp>
#include <boost/radix/detail/segment.hpp>

#include <boost/algorithm/cxx11/copy_n.hpp>
#include <boost/array.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

// -----------------------------------------------------------------------------
//
namespace boost { namespace radix {

// -----------------------------------------------------------------------------
//
namespace adl {

template <typename Segment, typename Codec>
auto unpack_segment(Segment const& segment, Codec const&)
{
    typedef big_endian_segment_unpacker<
        detail::required_bits<Codec::alphabet_size>::value,
        detail::unpacked_segment_size<Codec::alphabet_size>::value
    > unpacker;
    return unpacker()(segment);
}

} // namespace adl

// -----------------------------------------------------------------------------
//
namespace detail {

template <typename Alphabet>
struct bits_to_char_mapper
{
    bits_to_char_mapper(Alphabet const& alphabet)
        : alphabet_(alphabet)
    {}

    char_type operator()(bits_type bits) const
    {
        return alphabet_.char_from_bits(bits);
    }

    Alphabet const& alphabet_;
};

template <typename Alphabet>
bits_to_char_mapper<Alphabet> make_bits_to_char_mapper(Alphabet const& alphabet)
{
    return bits_to_char_mapper<Alphabet>(alphabet);
}

// cglover-todo: Specialize on iterator tag.
template <typename Iterator, typename EndIterator, typename Codec>
auto get_packed_segment(Iterator& first, EndIterator last, Codec const& codec)
{
    typedef boost::array<
        char_type,
        detail::packed_segment_size<Codec::alphabet_size>::value>
        packed_buffer_type;
    packed_buffer_type buffer;

    packed_buffer_type::iterator bfirst = buffer.begin();
    packed_buffer_type::iterator blast  = buffer.end();

    while(first != last && bfirst != blast)
    {
        *bfirst++ = *first++;
    }

    // Pad the result in case last-first < buffer.size();
    std::fill(bfirst, blast, codec.get_pad_bits());

    return buffer;
}

template <typename InputBuffer, typename Codec>
auto call_unpack_segment(InputBuffer const& buffer, Codec const& codec)
{
    using boost::radix::adl::unpack_segment;
    return unpack_segment(buffer, codec);
}

} // namespace detail

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
    while(first != last)
    {
        auto packed_segment =
            ::boost::radix::detail::get_packed_segment(first, last, codec);

        auto unpacked_segment =
            ::boost::radix::detail::call_unpack_segment(packed_segment, codec);

        out = std::transform(
            unpacked_segment.begin(), unpacked_segment.end(), out,
            detail::make_bits_to_char_mapper(codec));
    }

    // boost::array<unsigned char, 3> input_buffer;
    // boost::array<unsigned char, 4> output_buffer;
    // int i = 0;

    // while(first != last)
    // {
    //     input_buffer[i++] = *first++;
    //     if(i == 3)
    //     {
    //         output_buffer[0] = (input_buffer[0] & 0xfc) >> 2;
    //         output_buffer[1] = ((input_buffer[0] & 0x03) << 4) +
    //                            ((input_buffer[1] & 0xf0) >> 4);
    //         output_buffer[2] = ((input_buffer[1] & 0x0f) << 2) +
    //                            ((input_buffer[2] & 0xc0) >> 6);
    //         output_buffer[3] = input_buffer[2] & 0x3f;

    //         for(i = 0; (i < 4); ++i)
    //             *out++ =
    //             codec.get_alphabet().char_from_bits(output_buffer[i]);
    //         i = 0;
    //     }
    // }

    // if(i)
    // {
    //     for(int j = i; j < 3; ++j)
    //         input_buffer[j] = '\0';

    //     output_buffer[0] = (input_buffer[0] & 0xfc) >> 2;
    //     output_buffer[1] =
    //         ((input_buffer[0] & 0x03) << 4) + ((input_buffer[1] & 0xf0) >>
    //         4);
    //     output_buffer[2] =
    //         ((input_buffer[1] & 0x0f) << 2) + ((input_buffer[2] & 0xc0) >>
    //         6);
    //     output_buffer[3] = input_buffer[2] & 0x3f;

    //     for(int j = 0; (j < i + 1); ++j)
    //         *out++ = codec.get_alphabet().char_from_bits(output_buffer[j]);

    //     while(i++ < 3)
    //         *out++ = '=';
    // }
}
}} // namespace boost::radix

#endif // BOOST_RADIX_ENCODE_HPP