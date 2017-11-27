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

#include <boost/align/align_up.hpp>
#include <boost/assert.hpp>
#include <boost/integer/common_factor_rt.hpp>
#include <boost/radix/alphabet.hpp>
#include <limits>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace radix {

template <std::size_t AlphabetSize>
class codec
{
public:
    typedef alphabet<AlphabetSize> alphabet_type;

    codec(alphabet_type chars, char_type pad)
        : alphabet_(boost::move(chars))
        , pad_(pad)
    {}

    template <typename Iterator>
    codec(Iterator first, Iterator last, char_type pad)
        : alphabet(first, last)
        , pad_(pad)
    {}

    codec(char_type const* chars, char_type pad)
        : alphabet_(chars)
        , pad_(pad)
    {}

    alphabet_type const& get_alphabet() const { return alphabet_; }

private:
    alphabet<AlphabetSize> alphabet_;
    char pad_;
};

}} // namespace boost::radix

#endif // BOOST_RADIX_CODEC_HPP