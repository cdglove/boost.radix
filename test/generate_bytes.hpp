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
#include <boost/type_traits/make_unsigned.hpp>
#include <vector>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

inline std::vector<boost::radix::bits_type> generate_bytes(std::size_t num_bytes)
{
    std::vector<boost::radix::bits_type> binary;
    boost::random::mt19937 rng;
    boost::random::uniform_int_distribution<> bytes(0, 255);
    std::generate_n(std::back_inserter(binary), num_bytes, boost::bind(bytes, rng));
    return binary;
}

boost::radix::bits_type get_bits_natural(std::vector<boost::radix::bits_type> const& bytes, std::size_t bit_offset, std::size_t num_bits)
{
    using boost::radix::bits_type;
    std::size_t byte_offset = bit_offset / 8;
    std::size_t bit_in_byte = bit_offset % 8;
 
#error handle split read
    return (bits_type(bytes[byte_offset]) >> bit_in_byte) & (bits_type(~0) >> (8 - num_bits));
}

boost::radix::bits_type get_bits_sequencial(std::vector<boost::radix::bits_type> const& bytes, std::size_t bit_offset, std::size_t num_bits)
{
    using boost::radix::bits_type;
    std::size_t byte_offset = bit_offset / 8;
    std::size_t bit_in_byte = bit_offset % 8;
 
    return (bits_type(bytes[byte_offset]) >> (7 - bit_in_byte)) & (bits_type(~0) >> (8 - num_bits));
}

#endif // BOOST_RADIX_TEST_GENERATE_BYTES_HPP