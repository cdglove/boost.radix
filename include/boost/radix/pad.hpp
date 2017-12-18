//
// boost/radix/pad.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_PAD_HPP
#define BOOST_RADIX_PAD_HPP

#include <boost/radix/common.hpp>

#include <boost/radix/segment.hpp>
#include <boost/type_traits/integral_constant.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#    pragma once
#endif

namespace boost { namespace radix {

namespace detail {

template <std::size_t Bits>
struct requires_pad_impl;

template <>
struct requires_pad_impl<1>
{
    typedef boost::false_type type;
};

template <>
struct requires_pad_impl<2>
{
    typedef boost::false_type type;
};

template <>
struct requires_pad_impl<3>
{
    typedef boost::true_type type;
};

template <>
struct requires_pad_impl<4>
{
    typedef boost::false_type type;
};

template <>
struct requires_pad_impl<5>
{
    typedef boost::true_type type;
};

template <>
struct requires_pad_impl<6>
{
    typedef boost::true_type type;
};

template <>
struct requires_pad_impl<7>
{
    typedef boost::true_type type;
};

} // namespace detail

template <typename Codec>
struct requires_pad
{
    typedef typename detail::requires_pad_impl<required_bits<Codec>::value>::type type;
};

}} // namespace boost::radix

#endif // BOOST_RADIX_PAD_HPP
