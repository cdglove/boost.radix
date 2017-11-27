//
// boost/radix/detail/segment.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_DETAIL_SEGMENT_HPP
#define BOOST_RADIX_DETAIL_SEGMENT_HPP

#include <boost/config.hpp>

#include <boost/integer/common_factor.hpp>
#include <boost/integer/static_log2.hpp>

namespace boost { namespace radix {
namespace detail {

template <std::size_t AlphabetSize>
struct required_bits
{
    BOOST_STATIC_CONSTANT(
        std::size_t, value = boost::static_log2<AlphabetSize>::value);
};

template <std::size_t AlphabetSize>
struct bits_lcm
{
    typedef boost::integer::static_lcm<required_bits<AlphabetSize>::value, 8>
        type;
};

template <std::size_t AlphabetSize>
struct packed_segment_size
{
    BOOST_STATIC_CONSTANT(std::size_t, value = bits_lcm<AlphabetSize>::type::value / 8);
};

template <std::size_t AlphabetSize>
struct unpacked_segment_size
{
    BOOST_STATIC_CONSTANT(
        std::size_t,
        value = bits_lcm<AlphabetSize>::type::value / detail::required_bits<AlphabetSize>::value);
};

} // namespace detail

}} // namespace boost::radix

#endif // BOOST_RADIX_DETAIL_SEGMENT_HPP