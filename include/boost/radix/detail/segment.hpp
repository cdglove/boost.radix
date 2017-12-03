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

#include <boost/radix/common.hpp>

#include <boost/integer/common_factor.hpp>
#include <boost/integer/static_log2.hpp>

namespace boost { namespace radix {

namespace detail {

// bits_lcm exists only to keep a comma out of BOOST_STATIC_CONSTANT macros
template <std::size_t Bits>
struct bits_lcm
{
    typedef typename boost::integer::static_lcm<Bits, 8> type;
};

} // namespace detail

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
        value = detail::bits_lcm<required_bits<Codec>::value>::type::value / 8);
};

template <typename Codec>
struct unpacked_segment_size
{
    BOOST_STATIC_CONSTANT(
        std::size_t,
        value = detail::bits_lcm<required_bits<Codec>::value>::type::value /
                required_bits<Codec>::value);
};

}} // namespace boost::radix

#endif // BOOST_RADIX_DETAIL_SEGMENT_HPP
