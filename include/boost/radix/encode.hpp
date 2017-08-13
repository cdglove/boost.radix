//
// boost/radix/encode.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_ENCODE_HPP
#define BOOST_RADIX_ENCODE_HPP

#include <boost/config.hpp>
#include <boost/align/align_up.hpp>
#include <boost/radix/alphabet.hpp>
#include <boost/radix/detail/ibitstream.hpp>
#include <boost/integer/common_factor_rt.hpp>
#include <limits>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace radix { 

std::size_t encoded_size(std::size_t original_size, alphabet const& scheme)
{
    // Make sure we're not going to overflow the size.    
    BOOST_ASSERT(original_size < (std::numeric_limits<std::size_t>::max() / 8));
    std::size_t encoded_quanta = boost::integer::lcm(std::size_t(8), scheme.bits_required());

    std::size_t original_bits = original_size * 8;
    std::size_t spill = original_bits % encoded_quanta;

    if(spill)
    {
        original_bits += (encoded_quanta - spill);
    }

    std::size_t encoded_bytes = original_bits / scheme.bits_required();
    
    // Align up to minimum quanta size.
    return encoded_bytes;
}

template<typename InputIterator, typename OutputIterator>
void encode(InputIterator first, InputIterator last, 
            OutputIterator out, alphabet const& scheme) 
{
    detail::ibitstream<InputIterator> s(first, last);
    std::size_t num_bits = scheme.bits_required();
    while(!s.finished())
    {
        char c;
        s.read_bits(c, num_bits);
        *out++ = scheme.char_from_bits(c);
    }
}

}} // namespace boost { namespace radix { 

#endif // BOOST_RADIX_ENCODE_HPP