//
// boost/radix/codec_traits/validation.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_CODECTRAITS_VALIDATION_HPP
#define BOOST_RADIX_CODECTRAITS_VALIDATION_HPP

#include <boost/radix/common.hpp>

#include <boost/type_traits/integral_constant.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#    pragma once
#endif

namespace boost { namespace radix {

namespace handler {

struct assert
{};

struct exception
{};

struct ignore
{};

} // namespace tag

namespace codec_traits {

template <typename Codec>
struct on_nonalphabet_char
{
    typedef handler::assert type;
};

template <typename Codec>
struct on_whitespace_char
{
    typedef handler::assert type;
};

} // namespace codec_traits
}} // namespace boost::radix

#endif // BOOST_RADIX_CODECTRAITS_REQUIRESVALIDATION_HPP
