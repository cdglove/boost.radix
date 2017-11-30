//
// boost/radix/bitmask.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_BITMASK_HPP
#define BOOST_RADIX_BITMASK_HPP

#include <boost/config.hpp>

#include <boost/integer/integer_mask.hpp>
#include <boost/radix/common.hpp>

namespace boost { namespace radix {

template <std::size_t Bits>
struct mask
{
    BOOST_STATIC_CONSTANT(
        typename low_bits_mask_t<Bits>::fast,
        value = low_bits_mask_t<Bits>::sig_bits_fast);
};

template <std::size_t Bits, std::size_t Shift>
struct mask_shift
{
    BOOST_STATIC_CONSTANT(
        typename low_bits_mask_t<Bits>::fast,
        value = low_bits_mask_t<Bits>::sig_bits_fast << Shift);
};

}} // namespace boost::radix

#endif // BOOST_RADIX_BITMASK_HPP
