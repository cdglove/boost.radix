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

#include <boost/bind.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <vector>

typedef boost::radix::bits_type bits_type;
typedef boost::radix::char_type char_type;

#ifdef BOOST_HAS_PRAGMA_ONCE
#    pragma once
#endif

inline std::vector<bits_type>
generate_random_bytes(std::size_t num_bytes, int max_value = 255)
{
    std::vector<bits_type> binary;
    boost::random::mt19937 rng;
    boost::random::uniform_int_distribution<> bytes(0, max_value);
    std::generate_n(
        std::back_inserter(binary), num_bytes, boost::bind(bytes, rng));
    return binary;
}

inline std::vector<char_type> generate_alphabet(std::size_t bits)
{
    BOOST_ASSERT(bits < 8);
    std::vector<char_type> alphabet;
    char_type c = 0;
    for(bits_type i = 0; i < (1 << bits); ++i)
    {
        BOOST_ASSERT(i <= c);
        alphabet.push_back(c);
        c += 2;
    }

    return alphabet;
}

inline std::vector<bits_type>
generate_01_bit_pattern_bytes(std::size_t num_bytes)
{
    std::vector<bits_type> binary(num_bytes);
    bits_type pattern = 0x55; // 01010101b;
    std::fill(binary.begin(), binary.end(), pattern);
    return binary;
}

inline bits_type get_bits_lsb(
    std::vector<bits_type> const& bytes,
    std::size_t bit_offset,
    std::size_t num_bits)
{
    std::size_t byte_offset = bit_offset / 8;
    std::size_t bit_in_byte = bit_offset % 8;
    std::size_t read_size   = std::min(8 - bit_in_byte, num_bits);

    bits_type ret_val = (bits_type(bytes[byte_offset]) >> bit_in_byte) &
                        (bits_type(~0) >> (8 - num_bits));
    if(read_size < num_bits)
    {
        std::size_t bits_remaining = num_bits - read_size;
        bits_type extra_bits       = bytes[byte_offset + 1];
        extra_bits &= bits_type(~0) >> (8 - bits_remaining);
        extra_bits <<= read_size;
        ret_val |= extra_bits;
    }

    return ret_val;
}

inline bits_type get_bits_msb(
    std::vector<bits_type> const& bytes,
    std::size_t bit_offset,
    std::size_t num_bits)
{
    std::size_t byte_offset = bit_offset / 8;
    std::size_t bit_in_byte = bit_offset % 8;
    std::size_t read_size   = std::min(8 - bit_in_byte, num_bits);

    bits_type ret_val = bits_type(bytes[byte_offset]);
    ret_val >>= 8 - read_size - bit_in_byte;
    ret_val &= bits_type(~0) >> (8 - read_size);

    if(read_size < num_bits)
    {
        std::size_t bits_remaining = num_bits - read_size;
        std::uint16_t temp         = ret_val;
        temp <<= 8;
        temp |= bytes[byte_offset + 1];
        temp >>= (8 - bits_remaining);
        ret_val = temp & bits_type(~0);
    }

    return ret_val;
}

inline void set_bits_lsb(
    std::vector<bits_type>& bytes,
    bits_type value,
    std::size_t bit_offset,
    std::size_t num_bits)
{
    std::size_t byte_offset = bit_offset / 8;
    std::size_t bit_in_byte = bit_offset % 8;

    std::uint16_t current_value = bytes[byte_offset];

    // This is only to guiard against reading off the end of the array
    if(bit_in_byte + num_bits > 8)
        current_value |= bytes[byte_offset+1] << 8;

    std::uint16_t mask = (~(std::uint16_t(~0) << num_bits)) << bit_in_byte;
    std::uint16_t value16 = std::uint16_t(value) << bit_in_byte;
   
    // Unset the bits we're about to write to.
    current_value &= ~mask;
    
    //  Set the bits we care about.
    current_value |= value16;
    bytes[byte_offset] = current_value & 0xff;

    if(bit_in_byte + num_bits > 8)
        bytes[byte_offset+1] = (current_value & 0xff00) >> 8;
}

inline void set_bits_msb(
    std::vector<bits_type>& bytes,
    bits_type value,
    std::size_t bit_offset,
    std::size_t num_bits)
{
    std::size_t byte_offset = bit_offset / 8;
    std::size_t bit_in_byte = bit_offset % 8;
   
    std::uint16_t current_value = bytes[byte_offset] << 8;

    // This is only to guiard against reading off the end of the array
    if(bit_in_byte + num_bits > 8)
        current_value |= std::uint16_t(bytes[byte_offset + 1]);

    std::uint16_t mask    = (~(std::uint16_t(~0) << num_bits)) << (16 - num_bits - bit_in_byte);
    std::uint16_t value16 = std::uint16_t(value) << (16 - num_bits - bit_in_byte);

    // Unset the bits we're about to write to.
    current_value &= ~mask;

    //  Set the bits we care about.
    current_value |= value16;
    bytes[byte_offset] = (current_value & 0xff00) >> 8;

    if(bit_in_byte + num_bits > 8)
        bytes[byte_offset + 1] = current_value & 0xff;
}

inline std::vector<bits_type> generate_all_permutations_msb(std::size_t bits)
{
    BOOST_ASSERT(bits < 8);
    int values = 1 << static_cast<int>(bits);
    std::vector<bits_type> data(((bits * values) + 7) / 8, 0);
    std::size_t offset = 0;
    for(bits_type i = 0; i < (1 << bits); ++i)
    {
        set_bits_msb(data, i, offset, bits);
        offset += bits;
    }

    return data;
}

inline std::vector<bits_type> generate_all_permutations_lsb(std::size_t bits)
{
    BOOST_ASSERT(bits < 8);
    int values = 1 << static_cast<int>(bits);
    std::vector<bits_type> data(((bits * values) + 7) / 8, 0);
    std::size_t offset = 0;
    for(bits_type i = 0; i < (1 << bits); ++i)
    {
        set_bits_lsb(data, i, offset, bits);
        offset += bits;
    }

    return data;
}

struct is_equal_unsigned
{
    template <typename T1, typename T2>
    bool operator()(T1 c1, T2 c2) const
    {
        return static_cast<unsigned char>(c1) == static_cast<unsigned char>(c2);
    }
};

#endif // BOOST_RADIX_TEST_GENERATE_BYTES_HPP
