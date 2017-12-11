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

#include <boost/radix/segment.hpp>
#include <boost/radix/static_ibitstream.hpp>

#include <boost/array.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

// -----------------------------------------------------------------------------
//
namespace boost { namespace radix {

template <typename Codec>
struct segment_unpacker_type
{
    typedef static_ibitstream<required_bits<Codec>::value> type;
};

// -----------------------------------------------------------------------------
//
namespace adl {

template <typename Codec, typename PackedSegment>
typename unpacked_segment_type<Codec>::type
unpack_segment(Codec const& codec, PackedSegment const& packed)
{
    typename unpacked_segment_type<Codec>::type unpacked;
    segment_unpacker_type<Codec>::type::unpack(packed, unpacked);
    return unpacked;
}

template <typename Codec>
std::size_t get_encoded_size(std::size_t source_size, Codec const& codec)
{
    // By default, size is an integer multiple of the output
    // segment size.
    return unpacked_segment_size<Codec>::value *
           (source_size + (packed_segment_size<Codec>::value - 1) /
                              packed_segment_size<Codec>::value);
}

} // namespace adl

// -----------------------------------------------------------------------------
//
namespace detail {

template <typename Codec>
class packed_segment_result
{
public:

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
		return begin() + size();
    }

	void pop_back()
	{
		--size_;
	}

    std::size_t size() const
    {
        return size_;
    }

private:

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

template <typename Iterator, typename EndIterator, typename Codec>
packed_segment_result<Codec>
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

template <typename Codec, typename InputBuffer>
typename unpacked_segment_type<Codec>::type
unpack_and_pad_segment(Codec const& codec, InputBuffer const& buffer)
{
    using boost::radix::adl::unpack_segment;
    typename unpacked_segment_type<Codec>::type unpacked =
        unpack_segment(codec, buffer);

    std::size_t pad = buffer.size();
    while(pad < packed_segment_size<Codec>::value)
        unpacked[++pad] = codec.get_pad_bits();

    return unpacked;
}

} // namespace detail

// -----------------------------------------------------------------------------
//
template <typename Codec>
std::size_t encoded_size(std::size_t source_size, Codec const& codec)
{
    using boost::radix::adl::get_encoded_size;
    get_encoded_size(source_size, codec);
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
    while(first != last)
    {
        typename detail::packed_segment_result<Codec> packed_segment =
            ::boost::radix::detail::get_packed_segment(first, last, codec);

        typename unpacked_segment_type<Codec>::type unpacked_segment =
            ::boost::radix::detail::unpack_and_pad_segment(
                codec, packed_segment);

        out = std::transform(
            unpacked_segment.begin(), unpacked_segment.end(), out,
            ::boost::radix::detail::bits_to_char_mapper<Codec>(codec));
    }
}
}} // namespace boost::radix

#endif // BOOST_RADIX_ENCODE_HPP
