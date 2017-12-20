//
// boost/radix/codec_base.hpp
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
    codec(
        Iterator first,
        Iterator last,
        char_type pad_char,
        bits_type pad_bits = ~bits_type(0))
        : alphabet_type(first, last, pad_char, pad_bits)
    {}

    explicit codec(boost::basic_string_view<char_type> chars)
        : alphabet_type(chars)
    {}

    codec(
        boost::basic_string_view<char_type> chars,
        char_type pad_char,
        bits_type pad_bits = ~bits_type(0))
        : alphabet_type(chars, pad_char, pad_bits)
    {}
};

}} // namespace boost::radix

#endif // BOOST_RADIX_CODEC_HPP
