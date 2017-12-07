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

#include <boost/radix/common.hpp>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/bind.hpp>
#include <vector>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

inline std::vector<boost::radix::bits_type> generate_bytes(std::size_t num_bytes, int max_value = 255)
{
    std::vector<boost::radix::bits_type> binary;
    boost::random::mt19937 rng;
    boost::random::uniform_int_distribution<> bytes(0, max_value);
    std::generate_n(std::back_inserter(binary), num_bytes, boost::bind(bytes, rng));
    return binary;
}

inline boost::radix::bits_type get_bits_natural(std::vector<boost::radix::bits_type> const& bytes, std::size_t bit_offset, std::size_t num_bits)
{
    using boost::radix::bits_type;
    std::size_t byte_offset = bit_offset / 8;
    std::size_t bit_in_byte = bit_offset % 8;
    std::size_t read_size = std::min(8 - bit_in_byte, num_bits);

    bits_type ret_val = (bits_type(bytes[byte_offset]) >> bit_in_byte) & (bits_type(~0) >> (8 - num_bits));
    if(read_size < num_bits)
    {
        std::size_t bits_remaining = num_bits - read_size;
        bits_type extra_bits = bytes[byte_offset+1];
        extra_bits &= bits_type(~0) >> (8 - bits_remaining);
        extra_bits <<= read_size;
        ret_val |= extra_bits;
    }

    return ret_val;
}

inline boost::radix::bits_type get_bits_sequential(std::vector<boost::radix::bits_type> const& bytes, std::size_t bit_offset, std::size_t num_bits)
{
    using boost::radix::bits_type;
    std::size_t byte_offset = bit_offset / 8;
    std::size_t bit_in_byte = bit_offset % 8;
    std::size_t read_size = std::min(8 - bit_in_byte, num_bits);
 
    bits_type ret_val = bits_type(bytes[byte_offset]);
    ret_val >>= 8 - read_size - bit_in_byte;
    ret_val &= bits_type(~0) >> (8 - read_size);

    if(read_size < num_bits)
    {
        std::size_t bits_remaining = num_bits - read_size;
        std::uint16_t temp = ret_val;
        temp <<= 8;
        temp |= bytes[byte_offset+1];
        temp >>= (8 - bits_remaining);
        ret_val = temp & bits_type(~0);
    }

    return ret_val;
}

#endif // BOOST_RADIX_TEST_GENERATE_BYTES_HPP