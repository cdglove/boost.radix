//
// test/ibstream.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE TestBitstreams
#include <boost/test/unit_test.hpp>

#include "generate_bytes.hpp"
#include <boost/array.hpp>
#include <boost/radix/detail/bits_lcm.hpp>
#include <boost/radix/static_ibitstream.hpp>
#include <boost/radix/static_sequential_ibitstream.hpp>
#include <vector>

template <std::size_t Bits>
static void check_static_istream(std::size_t num_segments)
{
    std::size_t const packed_segment_size =
        boost::radix::detail::bits_lcm<Bits>::value / 8;
    std::size_t const unpacked_segment_size =
        boost::radix::detail::bits_lcm<Bits>::value / Bits;
    std::vector<boost::radix::bits_type> packed =
        generate_bytes(num_segments * packed_segment_size);

    std::vector<boost::radix::bits_type> unpacked(unpacked_segment_size);

    for(std::size_t i = 0; i < num_segments; ++i)
    {
        typename boost::radix::static_ibitstream<Bits>::unpack(
            packed, unpacked);
        for(std::size_t j = 0, bit = 0; j < unpacked_segment_size;
            ++j, bit += Bits)
        {
            boost::radix::bits_type source_bits =
                get_bits_natural(packed, bit, Bits);
            BOOST_TEST(source_bits == unpacked[j]);
        }
    }
}

template <std::size_t Bits>
static void check_static_sequential_istream(std::size_t num_segments)
{
    std::size_t const packed_segment_size =
        boost::radix::detail::bits_lcm<Bits>::value / 8;
    std::size_t const unpacked_segment_size =
        boost::radix::detail::bits_lcm<Bits>::value / Bits;
    std::vector<boost::radix::bits_type> packed =
        generate_bytes(num_segments * packed_segment_size);

    std::vector<boost::radix::bits_type> unpacked(unpacked_segment_size);

    for(std::size_t i = 0; i < num_segments; ++i)
    {
        typename boost::radix::static_sequential_ibitstream<Bits>::unpack(
            packed, unpacked);
        for(std::size_t j = 0, bit = 0; j < unpacked_segment_size;
            ++j, bit += Bits)
        {
            boost::radix::bits_type source_bits =
                get_bits_sequential(packed, bit, Bits);
            BOOST_TEST(source_bits == unpacked[j]);
        }
    }
}

template <std::size_t Bits>
static void check_static_ostream(std::size_t num_segments)
{
    std::size_t const packed_segment_size =
        boost::radix::detail::bits_lcm<Bits>::value / 8;
    std::size_t const unpacked_segment_size =
        boost::radix::detail::bits_lcm<Bits>::value / Bits;
    std::vector<boost::radix::bits_type> packed =
        generate_bytes(num_segments * packed_segment_size);

    std::vector<boost::radix::bits_type> unpacked(unpacked_segment_size);

    for(std::size_t i = 0; i < num_segments; ++i)
    {
        typename boost::radix::static_ibitstream<Bits>::unpack(
            packed, unpacked);
        for(std::size_t j = 0, bit = 0; j < unpacked_segment_size;
            ++j, bit += Bits)
        {
            boost::radix::bits_type source_bits =
                get_bits_natural(packed, bit, Bits);
            BOOST_TEST(source_bits == unpacked[j]);
        }
    }
}

template <std::size_t Bits>
static void check_static_sequential_ostream(std::size_t num_segments)
{
    std::size_t const packed_segment_size =
        boost::radix::detail::bits_lcm<Bits>::value / 8;
    std::size_t const unpacked_segment_size =
        boost::radix::detail::bits_lcm<Bits>::value / Bits;
    std::vector<boost::radix::bits_type> packed =
        generate_bytes(num_segments * packed_segment_size);

    std::vector<boost::radix::bits_type> unpacked(unpacked_segment_size);

    for(std::size_t i = 0; i < num_segments; ++i)
    {
        typename boost::radix::static_sequential_ibitstream<Bits>::unpack(
            packed, unpacked);
        for(std::size_t j = 0, bit = 0; j < unpacked_segment_size;
            ++j, bit += Bits)
        {
            boost::radix::bits_type source_bits =
                get_bits_sequential(packed, bit, Bits);
            BOOST_TEST(source_bits == unpacked[j]);
        }
    }
}

BOOST_AUTO_TEST_CASE(static_ibitstream)
{
    // Testing 1 segment lets us catch buffer overflows.
    // Testing 10 gives us more data and tests the segment boundaries.
    boost::array<std::size_t, 2> segments = {1, 10};
    for(int i = 0; i < segments.size(); ++i)
    {
        check_static_istream<1>(segments[i]);
        check_static_istream<2>(segments[i]);
        check_static_istream<3>(segments[i]);
        check_static_istream<4>(segments[i]);
        check_static_istream<5>(segments[i]);
        check_static_istream<6>(segments[i]);
        check_static_istream<7>(segments[i]);
    }
}

BOOST_AUTO_TEST_CASE(static_sequential_ibitstream)
{
    // Testing 1 segment lets us catch buffer overflows.
    // Testing 10 gives us more data and tests the segment boundaries.
    boost::array<std::size_t, 2> segments = {1, 10};
    for(int i = 0; i < segments.size(); ++i)
    {
        check_static_sequential_istream<1>(segments[i]);
        check_static_sequential_istream<2>(segments[i]);
        check_static_sequential_istream<3>(segments[i]);
        check_static_sequential_istream<4>(segments[i]);
        check_static_sequential_istream<5>(segments[i]);
        check_static_sequential_istream<6>(segments[i]);
        check_static_sequential_istream<7>(segments[i]);
    }
}

BOOST_AUTO_TEST_CASE(static_ibitstream)
{
    // Testing 1 segment lets us catch buffer overflows.
    // Testing 10 gives us more data and tests the segment boundaries.
    boost::array<std::size_t, 2> segments = {1, 10};
    for(int i = 0; i < segments.size(); ++i)
    {
        check_static_ostream<1>(segments[i]);
        check_static_ostream<2>(segments[i]);
        check_static_ostream<3>(segments[i]);
        check_static_ostream<4>(segments[i]);
        check_static_ostream<5>(segments[i]);
        check_static_ostream<6>(segments[i]);
        check_static_ostream<7>(segments[i]);
    }
}

BOOST_AUTO_TEST_CASE(static_sequential_ibitstream)
{
    // Testing 1 segment lets us catch buffer overflows.
    // Testing 10 gives us more data and tests the segment boundaries.
    boost::array<std::size_t, 2> segments = {1, 10};
    for(int i = 0; i < segments.size(); ++i)
    {
        check_static_sequential_ostream<1>(segments[i]);
        check_static_sequential_ostream<2>(segments[i]);
        check_static_sequential_ostream<3>(segments[i]);
        check_static_sequential_ostream<4>(segments[i]);
        check_static_sequential_ostream<5>(segments[i]);
        check_static_sequential_ostream<6>(segments[i]);
        check_static_sequential_ostream<7>(segments[i]);
    }
}