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

#include <boost/algorithm/cxx11/copy_n.hpp>
#include <boost/array.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

// -----------------------------------------------------------------------------
//
namespace boost { namespace radix {

template <typename Codec>
struct packed_segment_type
{
    typedef boost::container::static_vector<
        bits_type,
        detail::packed_segment_size<Codec::alphabet_size>::value>
        type;
};

template <typename Codec>
struct unpacked_segment_type
{
    typedef boost::container::static_vector<
        bits_type,
        detail::packed_segment_size<Codec::alphabet_size>::value>
        type;
};

// -----------------------------------------------------------------------------
//
namespace adl {

template <typename Segment, typename Codec>
typename unpacked_segment_type<Codec>::type unpack_segment(Segment const& segment, Codec const&)
{
    typedef big_endian_segment_unpacker<
        detail::required_bits<Codec::alphabet_size>::value,
        detail::unpacked_segment_size<Codec::alphabet_size>::value>
        unpacker;
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

// cglover-todo: Specialize on iterator tag?
template <typename Iterator, typename EndIterator, typename Codec>
typename packed_segment_type<Codec>::type
get_packed_segment(Iterator& first, EndIterator last, Codec const& codec)
{
    packed_segment_type<Codec>::type buffer;

    while(first != last && buffer.size() < buffer.capacity())
    {
        buffer.push_back(*first++);
    }

    return buffer;
}

template <typename InputBuffer, typename Codec>
typename unpacked_segment_type<Codec>::type call_unpack_segment(InputBuffer const& buffer, Codec const& codec)
{
    using boost::radix::adl::unpack_segment;
    typename unpacked_segment_type<Codec>::type unpacked = unpack_segment(buffer, codec);
    while(unpacked.size() < unpacked.capacity())
    {
        unpacked.push_back(codec.get_pad_bits());
    }
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
        typename packed_segment_type<Codec>::type packed_segment =
            ::boost::radix::detail::get_packed_segment(first, last, codec);

        typename unpacked_segment_type<Codec>::type unpacked_segment =
            ::boost::radix::detail::call_unpack_segment(packed_segment, codec);

        out = std::transform(
            unpacked_segment.begin(), unpacked_segment.end(), out,
            detail::make_bits_to_char_mapper(codec));
    }
}
}} // namespace boost::radix

#endif // BOOST_RADIX_ENCODE_HPP
