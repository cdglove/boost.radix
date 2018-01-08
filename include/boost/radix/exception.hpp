//
// boost/radix/exception.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_EXCEPTION_HPP
#define BOOST_RADIX_EXCEPTION_HPP

#include <boost/radix/common.hpp>

#include <boost/exception/exception.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#    pragma once
#endif

namespace boost { namespace radix {

class nonalphabet_character : public boost::exception
{};

class invalid_whitespace : public boost::exception
{};

}} // namespace boost::radix

#endif // BOOST_RADIX_EXCEPTION_HPP
