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

#include <boost/radix/codec_traits/pad.hpp>
#include <boost/radix/codec_traits/segment.hpp>
#include <boost/radix/codec_traits/validation.hpp>
#include <boost/radix/exception.hpp>
#include <boost/radix/static_obitstream_msb.hpp>

#include <cctype>

#ifdef BOOST_HAS_PRAGMA_ONCE
#    pragma once
#endif

namespace boost { namespace radix {

namespace detail {

template <typename Codec>
bool handle_whitespace_character(
    Codec const& codec, char_type c, handler::assert)
{
    BOOST_ASSERT(!std::isspace(c));
    return true;
}

template <typename Codec>
bool handle_whitespace_character(
    Codec const& codec, char_type c, handler::exception)
{
    if(std::isspace(c))
    {
        BOOST_THROW_EXCEPTION(invalid_whitespace());
    }
    return true;
}

template <typename Codec>
bool handle_whitespace_character(
    Codec const& codec, char_type c, handler::ignore)
{
    if(std::isspace(c))
        return false;
    return true;
}

template <typename Codec>
bool handle_nonalphabet_character(
    Codec const& codec, char_type c, handler::assert)
{
    BOOST_ASSERT(codec.has_char(c));
    return true;
}

template <typename Codec>
bool handle_nonalphabet_character(
    Codec const& codec, char_type c, handler::exception)
{
    if(!codec.has_char(c))
    {
        BOOST_THROW_EXCEPTION(nonalphabet_character());
    }
    return true;
}

template <typename Codec>
bool handle_nonalphabet_character(
    Codec const& codec, char_type c, handler::ignore)
{
    if(!codec.has_char(c))
        return false;
    return true;
}

} // namespace detail

// -----------------------------------------------------------------------------
//
namespace adl {

template <typename Codec>
static_obitstream_msb<codec_traits::required_bits<Codec>::value>
get_segment_packer(Codec const& codec)
{
    return static_obitstream_msb<codec_traits::required_bits<Codec>::value>();
}

template <typename Codec>
std::size_t get_decoded_size(std::size_t source_size, Codec const& codec)
{
    using boost::radix::codec_traits::packed_segment_size;
    using boost::radix::codec_traits::unpacked_segment_size;

    // By default, size is an integer multiple of the output
    // segment size.
    return packed_segment_size<Codec>::value *
           (source_size + (unpacked_segment_size<Codec>::value - 1) /
                              unpacked_segment_size<Codec>::value);
}

template <typename Codec>
bool validate_nonalphabet_character(Codec const& codec, char_type c)
{
    return ::boost::radix::detail::handle_nonalphabet_character(
        codec, c, typename codec_traits::on_whitespace_char<Codec>::type());
}

template <typename Codec>
bool validate_whitespace_character(Codec const& codec, char_type c)
{
    return ::boost::radix::detail::handle_whitespace_character(
        codec, c, typename codec_traits::on_nonalphabet_char<Codec>::type());
}

template <typename Codec>
bool validate_character(Codec const& codec, char_type c)
{
    return validate_whitespace_character(codec, c) &&
           validate_nonalphabet_character(codec, c);
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
    UnpackedSegment& unpacked)
{
    using boost::radix::codec_traits::required_bits;

    typename UnpackedSegment::iterator ubegin = unpacked.begin();
    typename UnpackedSegment::iterator uend   = ubegin + unpacked.capacity();

    while(first != last && ubegin != uend)
    {
        char_type c = *first++;

        using boost::radix::adl::validate_character;
        if(validate_character(codec, c))
            *ubegin++ = codec.bits_from_char(c);
    }

    unpacked.resize(std::distance(unpacked.begin(), ubegin));
    std::fill(ubegin, uend, 0);
}

// -----------------------------------------------------------------------------
//
template <
    typename InputIterator,
    typename InputEndIterator,
    typename OutputIterator,
    typename Codec,
    typename SegmentPacker>
std::size_t decode_impl(
    InputIterator first,
    InputEndIterator last,
    OutputIterator out,
    Codec const& codec,
    SegmentPacker packer)
{
    if(first == last)
        return 0;

    using boost::radix::codec_traits::packed_segment_size;
    using boost::radix::codec_traits::required_bits;
    using boost::radix::codec_traits::unpacked_segment_size;

    std::size_t bytes_written = 0;
    std::size_t ins           = 0;

    while(true)
    {
        detail::segment_buffer<bits_type, unpacked_segment_size<Codec>::value>
            unpacked_segment;
        ::boost::radix::detail::get_unpacked_segment(
            first, last, codec, unpacked_segment);

        ins += unpacked_segment.size();

        boost::array<bits_type, packed_segment_size<Codec>::value>
            packed_segment;
        packer(unpacked_segment, packed_segment);

        // If we hit the end, we can't write out everything.
        if(first == last)
        {
            typename detail::segment_buffer<
                bits_type, unpacked_segment_size<Codec>::value>::iterator pad =
                std::find(
                    unpacked_segment.begin(), unpacked_segment.end(),
                    codec.get_pad_bits());

            std::size_t output_size = 1;

            if(codec_traits::required_bits<Codec>::value > 1)
            {
                output_size = (std::distance(unpacked_segment.begin(), pad) *
                               codec_traits::required_bits<Codec>::value) /
                              8;
            }

            std::copy(
                packed_segment.begin(), packed_segment.begin() + output_size,
                out);

            bytes_written += output_size;
            break;
        }

        out = std::copy(packed_segment.begin(), packed_segment.end(), out);
        bytes_written += packed_segment.size();
    }

    return bytes_written;
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
    typename Codec>
std::size_t decode(
    InputIterator first,
    InputEndIterator last,
    OutputIterator out,
    Codec const& codec)
{
    using boost::radix::adl::get_segment_packer;
    return boost::radix::detail::decode_impl(
        first, last, out, codec, get_segment_packer(codec));
}

}} // namespace boost::radix

#endif // BOOST_RADIX_DECODE_HPP
