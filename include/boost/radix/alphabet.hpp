//
// boost/radix/alphabet.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_ALPHABET_HPP
#define BOOST_RADIX_ALPHABET_HPP

#include <boost/radix/common.hpp>

#include <boost/array.hpp>
#include <boost/static_assert.hpp>
#include <boost/utility/string_view.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#    pragma once
#endif

namespace boost { namespace radix {

template <std::size_t Size>
class alphabet
{
public:
    BOOST_STATIC_ASSERT(Size < std::size_t(1) << 63);
    BOOST_STATIC_CONSTANT(std::size_t, alphabet_size = Size);

    template <typename Iterator>
    alphabet(Iterator first, Iterator last)
    {
        init_from_iterators(first, last, bits_type(~0U), '=');
    }

    template <typename Iterator>
    alphabet(
        Iterator first, Iterator last, bits_type pad_bits, char_type pad_char)
    {
        init_from_iterators(first, last, pad_bits, pad_char);
    }

    explicit alphabet(boost::basic_string_view<char_type> chars)
    {
        init_from_iterators(chars.begin(), chars.end(), bits_type(~0U), '=');
    }

    alphabet(
        boost::basic_string_view<char_type> chars,
        bits_type pad_bits,
        char_type pad_char)
    {
        init_from_iterators(chars.begin(), chars.end());
    }

    char_type char_from_bits(bits_type index) const
    {
        return chars_[index];
    }

    bits_type bits_from_char(char_type index) const
    {
        return bits_[(unsigned char)(index)];
    }

    char_type get_pad_char() const
    {
        return chars_[pad_bits_];
    }

    bits_type get_pad_bits() const
    {
        return pad_bits_;
    }

    void set_pads(bits_type pad_bits, char_type pad_char)
    {
        BOOST_ASSERT(pad_bits > Size);
        BOOST_ASSERT(
            std::find(chars_.begin(), chars_.end(), pad_char) == chars_.end());

        pad_bits_        = pad_bits;
        chars_[pad_bits] = pad_char;
        bits_[pad_char]  = pad_bits;
    }

private:
    template <typename Iterator>
    void init_from_iterators(
        Iterator first, Iterator last, bits_type pad_bits, char_type pad_char)
    {
        std::fill(std::copy(first, last, chars_.begin()), chars_.end(), '\0');

        for(bits_type i = 0; i < Size; ++i)
        {
            bits_[static_cast<bits_type>(chars_[i])] = i;
        }

        set_pads(pad_bits, pad_char);
    }

    boost::array<char_type, 256> chars_;
    boost::array<bits_type, 256> bits_;
    bits_type pad_bits_;
};

}} // namespace boost::radix

#endif // BOOST_RADIX_ALPHABET_HPP
