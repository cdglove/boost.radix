//
// boost/radix/basic_codec_traits.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_BASICCODECTRAITS_HPP
#define BOOST_RADIX_BASICCODECTRAITS_HPP

#include <boost/radix/common.hpp>

#include <boost/type_traits/conditional.hpp>
#include <boost/type_traits/integral_constant.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#    pragma once
#endif

namespace boost { namespace radix {

template <typename Codec>
struct max_encoded_line_length
{
    BOOST_STATIC_CONSTANT(std::size_t, value = 0);
};

template <typename Codec>
struct requires_line_breaks
{
    typedef typename boost::conditional<
        max_encoded_line_length<Codec>::value,
        boost::true_type,
        boost::false_type>::type type;
};

}} // namespace boost::radix

#endif // BOOST_RADIX_BASICCODECTRAITS_HPP
