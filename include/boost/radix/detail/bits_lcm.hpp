//
// boost/radix/detail/segment.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_DETAIL_BITSLCM_HPP
#define BOOST_RADIX_DETAIL_BITSLCM_HPP

#include <boost/radix/common.hpp>

#include <boost/integer/common_factor.hpp>

namespace boost { namespace radix { namespace detail {

template <std::size_t Bits>
struct bits_lcm
{
    typedef typename boost::integer::static_lcm<Bits, 8> type;
    BOOST_STATIC_CONSTANT(std::size_t, value = type::value);
};

}}} // namespace boost::radix::detail

#endif // BOOST_RADIX_DETAIL_BITSLCM_HPP