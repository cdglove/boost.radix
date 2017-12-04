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

#include <boost/radix/detail/bits_lcm.hpp>

#include <boost/integer/static_log2.hpp>
#include <boost/array.hpp>

namespace boost { namespace radix {

template <typename Codec>
struct required_bits
{
    BOOST_STATIC_CONSTANT(
        std::size_t, value = boost::static_log2<Codec::alphabet_size>::value);
};

template <typename Codec>
struct packed_segment_size
{
    BOOST_STATIC_CONSTANT(
        std::size_t,
        value = detail::bits_lcm<required_bits<Codec>::value>::value / 8);
};

template <typename Codec>
struct unpacked_segment_size
{
    BOOST_STATIC_CONSTANT(
        std::size_t,
        value = detail::bits_lcm<required_bits<Codec>::value>::value /
                required_bits<Codec>::value);
};

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

}} // namespace boost::radix

#endif // BOOST_RADIX_SEGMENT_HPP
