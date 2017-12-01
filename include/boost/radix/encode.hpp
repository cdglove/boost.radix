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
#include <boost/radix/detail/segment.hpp>
#include <boost/radix/segment_unpacker.hpp>

#include <boost/array.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

// -----------------------------------------------------------------------------
//
namespace boost { namespace radix {

template <typename Codec>
struct packed_segment_type
{
    typedef boost::array<bits_type, packed_segment_size<Codec>::value> type;
};

template <typename Codec>
struct unpacked_segment_type
{
    typedef boost::array<bits_type, unpacked_segment_size<Codec>::value> type;
};

template <typename Codec>
struct segment_unpacker
{
    template <typename PackedSegment>
    static typename unpacked_segment_type<Codec>::type
    unpack(Codec const& codec, PackedSegment const& packed)
    {
        unpacked_segment_type<Codec>::type unpacked;
        big_endian_segment_unpacker<required_bits<Codec>::value>::unpack(
            packed, unpacked);
        return unpacked;
    }
};

// -----------------------------------------------------------------------------
//
namespace detail {

template <typename Codec>
struct packed_segment_result
{
    typedef typename packed_segment_type<Codec>::type container;
    typedef typename container::iterator iterator;
    typedef typename container::reference reference;
    typedef typename container::const_reference const_reference;

    packed_segment_result()
        : size_(packed_segment_size<Codec>::value)
    {}

    reference operator[](std::size_t idx)
    {
        return container_[idx];
    }

    const_reference operator[](std::size_t idx) const
    {
        return container_[idx];
    }

    iterator begin()
    {
        return container_.begin();
    }

    iterator end()
    {
        return container_.end();
    }

    std::size_t size() const
    {
        return size_;
    }

    container container_;
    std::size_t size_;
};

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

// cglover-todo: Specialize on iterator tag?
template <typename Iterator, typename EndIterator, typename Codec>
detail::packed_segment_result<Codec>
get_packed_segment(Iterator& first, EndIterator last, Codec const& codec)
{
    packed_segment_result<Codec> buffer;
    typename packed_segment_type<Codec>::type::iterator bbegin = buffer.begin();
    typename packed_segment_type<Codec>::type::iterator bend   = buffer.end();

    while(first != last && bbegin != bend)
    {
        *bbegin++ = *first++;
    }

    while(bbegin != bend)
    {
        *bbegin++ = 0;
        --buffer.size_;
    }

    return buffer;
}

template <typename InputBuffer, typename Codec>
typename unpacked_segment_type<Codec>::type
unpack_segment(InputBuffer const& buffer, Codec const& codec)
{
    typename unpacked_segment_type<Codec>::type unpacked =
        typename segment_unpacker<Codec>::unpack(codec, buffer);

    std::size_t pad = buffer.size();
    while(pad < packed_segment_size<Codec>::value)
        unpacked[++pad] = codec.get_pad_bits();

    return unpacked;
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
        typename detail::packed_segment_result<Codec> packed_segment =
            ::boost::radix::detail::get_packed_segment(first, last, codec);

        typename unpacked_segment_type<Codec>::type unpacked_segment =
            ::boost::radix::detail::unpack_segment(packed_segment, codec);

        out = std::transform(
            unpacked_segment.begin(), unpacked_segment.end(), out,
            detail::bits_to_char_mapper<Codec>(codec));
    }
}
}} // namespace boost::radix

#endif // BOOST_RADIX_ENCODE_HPP
