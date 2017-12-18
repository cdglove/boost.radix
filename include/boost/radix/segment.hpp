//
// boost/radix/detail/segment.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_SEGMENT_HPP
#define BOOST_RADIX_SEGMENT_HPP

#include <boost/radix/common.hpp>

#include <boost/radix/detail/bits.hpp>
#include <boost/radix/detail/segment_buffer.hpp>

#include <boost/array.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#    pragma once
#endif

namespace boost { namespace radix {

template <typename Codec>
struct required_bits
{
    BOOST_STATIC_CONSTANT(
        std::size_t, value = bits::from_alphabet_size<Codec::alphabet_size>::value);
};

template <typename Codec>
struct packed_segment_size
{
    BOOST_STATIC_CONSTANT(
        std::size_t,
        value = bits::to_packed_segment_size<required_bits<Codec>::value>::value);
};

template <typename Codec>
struct unpacked_segment_size
{
    BOOST_STATIC_CONSTANT(
        std::size_t,
        value = bits::to_unpacked_segment_size<required_bits<Codec>::value>::value);
};

}} // namespace boost::radix

#endif // BOOST_RADIX_SEGMENT_HPP
