//
// boost/radix/detail/bits.hpp
//
// Copyright (c) Chris Glover, 2017-2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_DETAIL_BITS_HPP
#define BOOST_RADIX_DETAIL_BITS_HPP

#include <boost/radix/common.hpp>

#include <boost/integer/common_factor.hpp>
#include <boost/integer/static_log2.hpp>

namespace boost { namespace radix {
namespace detail {

template <std::size_t Bits>
struct bits_lcm
{
    typedef typename boost::integer::static_lcm<Bits, 8> type;
    BOOST_STATIC_CONSTANT(std::size_t, value = type::value);
};

} // namespace detail

namespace bits {

template <std::size_t AlphabetSize>
struct from_alphabet_size
{
    BOOST_STATIC_CONSTANT(
        std::size_t, value = boost::static_log2<AlphabetSize>::value);
};

template <std::size_t Bits>
struct to_alphabet_size
{
    BOOST_STATIC_CONSTANT(
        std::size_t, value = 1 << Bits);
};

template <std::size_t Bits>
struct to_packed_segment_size
{
    BOOST_STATIC_CONSTANT(
        std::size_t, value = detail::bits_lcm<Bits>::value / 8);
};

template <std::size_t Bits>
struct to_unpacked_segment_size
{
    BOOST_STATIC_CONSTANT(
        std::size_t, value = detail::bits_lcm<Bits>::value / Bits);
};

} // namespace bits
}} // namespace boost::radix

#endif // BOOST_RADIX_DETAIL_BITS_HPP
