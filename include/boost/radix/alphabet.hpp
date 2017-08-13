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

#include <boost/config.hpp>
#include <boost/assert.hpp>
#include <boost/array.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace radix { 

class alphabet
{
public:

    template<typename Iterator>
    alphabet(Iterator first, Iterator last)
    {
        init_from_iterators(first, last);
    }

    template<typename Container>
    explicit alphabet(Container const& c)
    {
        init_from_iterators(c.begin(), c.end());
    }

    template<typename Char>
    explicit alphabet(Char const* chars)
    {
        Char const* last = chars;
        while(*last)
            ++last;

        init_from_iterators(chars, last);
    }

    std::size_t size() const
    {
        return alphabet_size_;
    }

    std::size_t bits_required() const
    {
        return bits_required_;
    }

    char char_from_bits(std::size_t index) const
    {
        return chars_[index];
    }

    char bits_from_char(char index) const
    {
        return bits_[index];
    }
    
private:

    template<typename Iterator>
    void init_from_iterators(Iterator first, Iterator last)
    {
        alphabet_size_ = 0;
        for(Iterator current = first; current != last; ++current)
        {
            chars_[alphabet_size_] = *current;
            bits_[static_cast<unsigned char>(chars_[alphabet_size_])] = static_cast<unsigned char>(alphabet_size_);
            ++alphabet_size_;
            if(alphabet_size_ == chars_.size())
            {
                BOOST_ASSERT(false); // throw??
            }
        }

        std::size_t bit = 1;
        bits_required_ = 0;
        BOOST_ASSERT(alphabet_size_ < (bit << 63));
        while(bit < alphabet_size_)
        {
            bit <<= 1;
            ++bits_required_;
        }
    }

    boost::array<unsigned char, 128> chars_;
    boost::array<unsigned char, 256> bits_;
    std::size_t alphabet_size_;
    std::size_t bits_required_;
};

}} // namespace boost { namespace radix { 

#endif // BOOST_RADIX_ALPHABET_HPP