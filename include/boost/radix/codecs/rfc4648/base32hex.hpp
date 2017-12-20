//
// boost/radix/codec/rfc468/base32hex.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Based on https://tools.ietf.org/html/rfc4648
//

#ifndef BOOST_RADIX_CODEC_RFC4648_BASE32HEX_HPP
#define BOOST_RADIX_CODEC_RFC4648_BASE32HEX_HPP

#include <boost/radix/common.hpp>

#include <boost/radix/codec.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#    pragma once
#endif

namespace boost { namespace radix { namespace codecs { namespace rfc4648 {

class base32hex : public codec<32>
{
public:
    base32hex()
        : codec("0123456789ABCDEFGHIJKLMNOPQRSTUV", '=')
    {}
};

}}}} // namespace boost::radix::codec::rfc4648

#endif // BOOST_RADIX_CODEC_RFC4648_BASE32HEX_HPP