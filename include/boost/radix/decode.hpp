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

namespace detail {

template <typename Codec>
struct unpacked_segment_result
{
	typedef typename unpacked_segment_type<Codec>::type container;
	typedef typename container::iterator iterator;
	typedef typename container::reference reference;
	typedef typename container::const_reference const_reference;

	unpacked_segment_result()
		: size_(unpacked_segment_size<Codec>::value)
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
struct char_to_bits_mapper
{
    bits_to_char_mapper(Codec const& codec)
        : codec_(codec)
    {}

    bits_type operator()(char_type c) const
    {
        return codec_.bits_from_char(c);
    }

    Codec const& codec_;
};

template <typename Iterator, typename EndIterator, typename Codec>
unpacked_segment_result<Codec>
get_unpacked_segment(Iterator& first, EndIterator last, Codec const& codec)
{
	unpacked_segment_result<Codec> buffer;
	typename packed_segment_type<Codec>::type::iterator bbegin = buffer.begin();
	typename packed_segment_type<Codec>::type::iterator bend = buffer.end();

	while (first != last && bbegin != bend)
	{
		*bbegin++ = *first++;
	}

	while (bbegin != bend)
	{
		*bbegin++ = '\0';
		--buffer.size_;
	}

	return buffer;
}

template <typename Codec, typename InputBuffer>
typename packed_segment_type<Codec>::type
unpad_and_pack_segment(Codec const& codec, InputBuffer const& buffer)
{
	using boost::radix::adl::pack_segment;
	typename packed_segment_type<Codec>::type unpacked =
		pack_segment(codec, buffer);

	std::size_t pad = buffer.size();
	while (pad < packed_segment_size<Codec>::value)
		unpacked[++pad] = codec.get_pad_bits();

	return unpacked;
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
    while(first != last)
    {
		typename detail::unpacked_segment_result<Codec> unpacked_segment =
			::boost::radix::detail::get_unpacked_segment(first, last, codec);

        typename detail::packed_segment_result<Codec> packed_segment =
            ::boost::radix::detail::pack_and_unpad_segment(codec);

        
    }

    // while (first != last)
    //{
    //	unsigned char current = *first++;
    //	if (current == codec.get_pad_char())
    //		break;

    //	input_buffer[i++] = current;
    //	if (i == 4)
    //	{
    //		for (i = 0; i < 4; ++i)
    //			input_buffer[i] = alpha.bits_from_char(input_buffer[i]);

    //		output_buffer[0] =
    //			(input_buffer[0] << 2) + ((input_buffer[1] & 0x30) >> 4);
    //		output_buffer[1] = ((input_buffer[1] & 0xf) << 4) +
    //			((input_buffer[2] & 0x3c) >> 2);
    //		output_buffer[2] = ((input_buffer[2] & 0x3) << 6) + input_buffer[3];

    //		for (i = 0; (i < 3); ++i)
    //			*out++ = output_buffer[i];
    //		i = 0;
    //	}
    //}

    // if (i)
    //{
    //	for (int j = i; j < 4; ++j)
    //		input_buffer[j] = 0;

    //	for (int j = 0; j < 4; ++j)
    //		input_buffer[j] = alpha.bits_from_char(input_buffer[j]);

    //	output_buffer[0] =
    //		(input_buffer[0] << 2) + ((input_buffer[1] & 0x30) >> 4);
    //	output_buffer[1] =
    //		((input_buffer[1] & 0xf) << 4) + ((input_buffer[2] & 0x3c) >> 2);
    //	output_buffer[2] = ((input_buffer[2] & 0x3) << 6) + input_buffer[3];

    //	for (int j = 0; (j < i - 1); ++j)
    //		*out++ = output_buffer[j];
    //}
}

}} // namespace boost::radix

#endif // BOOST_RADIX_DECODE_HPP
