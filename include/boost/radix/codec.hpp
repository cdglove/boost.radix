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

#include <boost/radix/common.hpp>

#include <boost/radix/alphabet.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#    pragma once
#endif

// -----------------------------------------------------------------------------
//
namespace boost { namespace radix {

template <std::size_t AlphabetSize, typename Tag = void>
class codec : public alphabet<AlphabetSize>
{
    typedef alphabet<AlphabetSize> alphabet_type;

public:
    template <typename Iterator>
    codec(Iterator first, Iterator last)
        : alphabet_type(first, last)
    {}

    template <typename Iterator>
    codec(Iterator first, Iterator last, bits_type pad_bits, char_type pad_char)
        : alphabet_type(first, last, pad_bits, pad_char)
    {}

    explicit codec(char_type const* chars)
        : alphabet_type(chars)
    {}

    codec(
        boost::basic_string_view<char_type> chars,
        bits_type pad_bits,
        char_type pad_char)
        : alphabet_type(chars, pad_bits, pad_char)
    {}
};

}} // namespace boost::radix

#endif // BOOST_RADIX_CODEC_HPP
