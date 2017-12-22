//
// boost/radix/codec/rfc468/base16.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_CODEC_RFC4648_BASE16_HPP
#define BOOST_RADIX_CODEC_RFC4648_BASE16_HPP

#include <boost/radix/common.hpp>

#include <boost/radix/basic_codec.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#    pragma once
#endif

namespace boost { namespace radix { namespace codec { namespace rfc4648 {

// Based on spec from https://tools.ietf.org/html/rfc4648
class base16 : public basic_codec<16>
{
public:
    base16()
        : basic_codec("0123456789ABDCEF")
    {}
};

}}}} // namespace boost::radix::codec::rfc4648

#endif // BOOST_RADIX_CODEC_RFC4648_BASE16_HPP