//
// boost/radix/codec.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_CODEC_HPP
#define BOOST_RADIX_CODEC_HPP

#include <boost/config.hpp>

#include <boost/radix/alphabet.hpp>
#include <boost/radix/common.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

// -----------------------------------------------------------------------------
//
namespace boost { namespace radix {

template <typename Tag, std::size_t AlphabetSize>
class codec : public alphabet<AlphabetSize>
{
public:
    template <typename Iterator>
    codec(Iterator first, Iterator last)
        : alphabet(first, last)
    {}

    template <typename Iterator>
    codec(Iterator first, Iterator last, bits_type pad_bits, char_type pad_char)
        : alphabet(first, last, pad_bits, pad_char)
    {}

    explicit codec(char_type const* chars)
        : alphabet(chars)
    {}

    codec(boost::basic_string_view<char_type> chars, bits_type pad_bits, char_type pad_char)
        : alphabet(chars, pad_bits, bad_char)
    {}
};

}} // namespace boost::radix

#endif // BOOST_RADIX_CODEC_HPP