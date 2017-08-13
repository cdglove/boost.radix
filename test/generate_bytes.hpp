//
// test/generate_bytes.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_TEST_GENERATE_BYTES_HPP
#define BOOST_RADIX_TEST_GENERATE_BYTES_HPP

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/bind.hpp>
#include <boost/type_traits/make_unsigned.hpp>
#include <vector>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

inline std::vector<char> generate_bytes(std::size_t num_bytes)
{
    std::vector<char> binary;
    boost::random::mt19937 rng;
    boost::random::uniform_int_distribution<> bytes(0, 255);
    std::generate_n(std::back_inserter(binary), num_bytes, boost::bind(bytes, rng));
    return binary;
}

template<typename T>
T get_bits(std::vector<char> const& bytes, std::size_t bit_offset, std::size_t num_bits)
{
    std::size_t byte_offset = bit_offset / 8;
    std::size_t bit_in_byte = bit_offset % 8;
 
    T ret = unsigned char(bytes[byte_offset]) >> bit_in_byte;
    
    std::size_t size_in_bits = sizeof(T) * 8;
    std::size_t bits_read = 8 - bit_in_byte;
    while(bits_read < num_bits)
    {
        ++byte_offset;
        T rest = unsigned char(bytes[byte_offset]);
        ret |= rest << bits_read;
        bits_read += 8;
    }

    boost::make_unsigned<T>::type mask = ~boost::make_unsigned<T>::type(0);
    mask >>= size_in_bits - num_bits;
    ret &= mask;
    return ret;
}

#endif // BOOST_RADIX_TEST_GENERATE_BYTES_HPP