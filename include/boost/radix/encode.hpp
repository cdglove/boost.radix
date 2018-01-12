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

#include <boost/radix/codec_traits/pad.hpp>
#include <boost/radix/codec_traits/segment.hpp>
#include <boost/radix/codec_traits/whitespace.hpp>
#include <boost/radix/static_ibitstream_msb.hpp>

#include <boost/array.hpp>
#include <boost/noncopyable.hpp>

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
    return codec_traits::unpacked_segment_size<Codec>::value *
           (source_size +
            (codec_traits::packed_segment_size<Codec>::value - 1) /
                codec_traits::packed_segment_size<Codec>::value);
}

template <typename Codec>
static_ibitstream_msb<codec_traits::required_bits<Codec>::value>
get_segment_unpacker(Codec const& codec)
{
    return static_ibitstream_msb<codec_traits::required_bits<Codec>::value>();
}

template <typename Codec, typename OutputIterator>
OutputIterator wrap_output_iterator(Codec const&, OutputIterator out)
{}

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

// -----------------------------------------------------------------------------
//
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
    typename PackedSegment::iterator pbegin = packed.begin();
    typename PackedSegment::iterator pend   = packed.end();

    while(first != last && pbegin != pend)
    {
        *pbegin++ = *first++;
    }

    packed.resize(std::distance(packed.begin(), pbegin));
    std::fill(pbegin, pend, 0);
}

// -----------------------------------------------------------------------------
//
template <typename Codec, typename PackedBuffer, typename UnpackedBuffer>
void maybe_pad_segment(
    Codec const& codec,
    PackedBuffer const& packed,
    UnpackedBuffer& unpacked,
    boost::true_type)
{
    using boost::radix::codec_traits::required_bits;

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

// -----------------------------------------------------------------------------
//
template <typename InnerIterator, std::size_t MaxLineLength>
struct line_break_iterator : std::iterator<std::output_iterator_tag, char_type>
{
    line_break_iterator(InnerIterator iter)
        : iter_(iter)
        , char_(0)
    {}

    line_break_iterator& operator++()
    {
        iter_++;
        return *this;
    }

    line_break_iterator& operator++(int)
    {
        ++iter_;
        return *this;
    }

    line_break_iterator& operator*()
    {
        return *this;
    }

    template <typename T>
    line_break_iterator& operator=(T const& t)
    {
        ++char_;
        if(char_ == MaxLineLength)
        {
            *iter_++ = '\n';
            char_    = 0;
        }

        *iter_ = t;
        return *this;
    }

    InnerIterator iter_;
    std::size_t char_;
};

template <typename Codec, typename OutputIterator>
line_break_iterator<
    OutputIterator,
    codec_traits::max_encoded_line_length<Codec>::value>
maybe_wrap_output_iterator(Codec const&, OutputIterator out, boost::true_type)
{
    return line_break_iterator<
        OutputIterator, codec_traits::max_encoded_line_length<Codec>::value>(
        out);
}

template <typename Codec, typename OutputIterator>
OutputIterator
maybe_wrap_output_iterator(Codec const&, OutputIterator out, boost::false_type)
{
    return out;
}

// -----------------------------------------------------------------------------
//
template <
    typename InputIterator,
    typename InputEndIterator,
    typename OutputIterator,
    typename Codec,
    typename SegmentUnpacker>
void encode_impl(
    InputIterator first,
    InputEndIterator last,
    OutputIterator out,
    Codec const& codec,
    SegmentUnpacker unpacker)
{
    using boost::radix::codec_traits::packed_segment_size;
    using boost::radix::codec_traits::unpacked_segment_size;

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
                typename codec_traits::requires_pad<Codec>::type());

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

} // namespace detail

template <typename Codec, typename OuputIterator, typename SegmentUnpacker>
class encoder : boost::noncopyable
{
public:
    encoder(Codec const& codec, OuputIterator out)
        : codec_(codec)
        , out_(out)
        , bytes_written_(0)
    {}

    template <typename Iterator, typename EndIterator>
    std::size_t append(Iterator first, EndIterator last)
    {
        using boost::radix::codec_traits::unpacked_segment_size;

        ::boost::radix::detail::get_packed_segment(
            first, last, codec_, packed_segment_);

        boost::array<char_type, unpacked_segment_size<Codec>::value>
            unpacked_segment;

        unpacker_(packed_segment_, unpacked_segment);

        if(first != last)
        {
            out_ = std::transform(
                unpacked_segment.begin(), unpacked_segment.end(), out,
                ::boost::radix::detail::bits_to_char_mapper<Codec>(codec_));

            bytes_written_ += unpacked_segment.static_size;
        }
    }

    std::size_t flush()
    {
        ::boost::radix::detail::maybe_pad_segment(
            codec_, packed_segment_, unpacked_segment,
            typename codec_traits::requires_pad<Codec>::type());

        out_ = std::transform(
            unpacked_segment.begin(), unpacked_segment.end(), out_,
            ::boost::radix::detail::bits_to_char_mapper<Codec>(codec_));
    }

    std::size_t bytes_written()
    {
        return bytes_written_;
    }

private:
    Codec const& codec_;
    OuputIterator out_;
    SegmentUnpacker unpacker_;
    std::size_t bytes_written_;

    detail::segment_buffer<
        bits_type,
        codec_traits::packed_segment_size<Codec>::value>
        packed_segment_;
}; // namespace radix

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
    typename Codec>
void encode(
    InputIterator first,
    InputEndIterator last,
    OutputIterator out,
    Codec const& codec)
{
    using boost::radix::adl::get_segment_unpacker;
    boost::radix::detail::encode_impl(
        first, last,
        ::boost::radix::detail::maybe_wrap_output_iterator(
            codec, out,
            typename codec_traits::requires_line_breaks<Codec>::type()),
        codec, get_segment_unpacker(codec));
}

}} // namespace boost::radix

#endif // BOOST_RADIX_ENCODE_HPP
