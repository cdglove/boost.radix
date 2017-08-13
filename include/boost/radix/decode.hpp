//
// boost/radix/decode.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_DECODE_HPP
#define BOOST_RADIX_DECODE_HPP

#include <boost/config.hpp>
#include <boost/radix/alphabet.hpp>
#include <boost/radix/detail/obitstream.hpp>
#include <limits>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace radix { 

std::size_t decoded_size(std::size_t encoded_size, alphabet const& chars)
{
    // Make sure we're not going to overflow the size.    
    BOOST_ASSERT(encoded_size < (std::numeric_limits<std::size_t>::max() / chars.bits_required()));
    std::size_t total_bits = encoded_size * chars.bits_required();

    total_bits = boost::alignment::align_up(total_bits, 8);
    std::size_t decoded_bytes = total_bits / 8;
    return decoded_bytes;
}

template<typename InputIterator, typename OutputIterator>
void decode(InputIterator first, InputIterator last, 
            OutputIterator out, alphabet const& scheme) 
{
    detail::obitstream<OutputIterator> s(out);
    std::size_t num_bits = scheme.bits_required();
    while(first != last)
    {
        s.write_bits(scheme.bits_from_char(*first++), num_bits);
    }
}

}} // namespace boost { namespace radix { 

#endif // BOOST_RADIX_DECODE_HPP