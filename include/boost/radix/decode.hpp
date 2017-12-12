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

template <typename Codec>
struct segment_packer_result
{
    typedef boost::array<bits_type, packed_segment_size<Codec>::value> type;
};

// -----------------------------------------------------------------------------
//
namespace adl {

template <typename Codec, typename UnpackedSegment>
typename segment_packer_result<Codec>::type
pack_segment(Codec const& codec, UnpackedSegment const& unpacked)
{
    typename segment_packer_result<Codec>::type packed;
    segment_packer_type<Codec>::type::pack(unpacked, packed);
    return packed;
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

template <typename Codec>
struct unpacked_segment_buffer_type
{
    typedef
        typename segment_buffer<bits_type, unpacked_segment_size<Codec>::value>
            type;
};

template <typename Iterator, typename EndIterator, typename Codec>
typename unpacked_segment_buffer_type<Codec>::type
get_unpacked_segment(Iterator& first, EndIterator last, Codec const& codec)
{
    typedef typename unpacked_segment_buffer_type<Codec>::type segment_buffer;
    segment_buffer buffer;
    segment_buffer::iterator bbegin = buffer.begin();
    segment_buffer::iterator bend   = buffer.end();

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

    return buffer;
}

template <typename Codec, typename InputBuffer>
typename segment_packer_result<Codec>::type
call_pack_segment(Codec const& codec, InputBuffer const& buffer)
{
    using boost::radix::adl::pack_segment;
    typename segment_packer_result<Codec>::type packed =
        pack_segment(codec, buffer);

    return packed;
}

} // namespace detail

template <typename Codec>
std::size_t decoded_size(std::size_t source_size, Codec const& codec)
{
    using boost::radix::adl::get_decoded_size;
    return get_decoded_size(source_size, codec);
}

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
    while(true)
    {
        typename detail::unpacked_segment_buffer_type<Codec>::type
            unpacked_segment = ::boost::radix::detail::get_unpacked_segment(
                first, last, codec);

        typename segment_packer_result<Codec>::type packed_segment =
            ::boost::radix::detail::call_pack_segment(codec, unpacked_segment);

        // If we hit the end, we can't write out everything.
        if(first == last)
        {
            std::size_t total_bits =
                unpacked_segment.size() * required_bits<Codec>::value;

            std::size_t total_bytes = ((total_bits + 7) / 8) - 1;

            std::copy(
                packed_segment.begin(), packed_segment.begin() + total_bytes,
                out);

            break;
        }

        out = std::copy(packed_segment.begin(), packed_segment.end(), out);
    }
}

}} // namespace boost::radix

#endif // BOOST_RADIX_DECODE_HPP
