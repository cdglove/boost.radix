//
// boost/radix/common.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_COMMON_HPP
#define BOOST_RADIX_COMMON_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif

namespace boost { namespace radix {

typedef unsigned char bits_type;
typedef char char_type;

}} // namespace boost::radix

#ifndef BOOST_RADIX_SUPPORT_STDERRORCODE
#  ifndef BOOST_NO_CXX11_HDR_SYSTEM_ERROR
#    define BOOST_RADIX_SUPPORT_STDERRORCODE 1
#  endif
#endif

#ifndef BOOST_RADIX_SUPPORT_BOOSTERRORCODE
#  define BOOST_RADIX_SUPPORT_BOOSTERRORCODE 1
#endif

#endif // BOOST_RADIX_COMMON_HPP
