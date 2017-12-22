//
// boost/radix/codec/rfc468/base64.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_CODEC_RFC4648_BASE64_HPP
#define BOOST_RADIX_CODEC_RFC4648_BASE64_HPP

#include <boost/radix/common.hpp>

#include <boost/radix/basic_codec.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#    pragma once
#endif

namespace boost { namespace radix { namespace codec { namespace rfc4648 {

// Based on spec from https://tools.ietf.org/html/rfc4648
class base64 : public basic_codec<64>
{
public:
    base64()
        : basic_codec("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/", '=')
    {}
};

}}}} // namespace boost::radix::codec::rfc4648

#endif // BOOST_RADIX_CODEC_RFC4648_BASE64_HPP