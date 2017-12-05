//
// test/ibstream.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE TestStaticInBitstream
#include <boost/test/unit_test.hpp>

#include <boost/radix/static_ibitstream.hpp>
#include <boost/radix/detail/bits_lcm.hpp>
#include "generate_bytes.hpp"
#include <vector>

template<std::size_t Bits>
static void check_bits()
{
    int const num_segments = 3;
    std::size_t const packed_segment_size = boost::radix::detail::bits_lcm<Bits>::value / 8;
    std::size_t const unpacked_segment_size = boost::radix::detail::bits_lcm<Bits>::value / Bits;
    std::vector<boost::radix::bits_type> packed = generate_bytes(num_segments * packed_segment_size);

    std::vector<boost::radix::bits_type> unpacked(unpacked_segment_size);

    for(std::size_t i = 0; i < num_segments; ++i)
    {
        typename boost::radix::static_ibitstream<Bits>::unpack(packed, unpacked);
        for(std::size_t j = 0, bit = 0; j < unpacked_segment_size; ++j, bit += Bits)
        {
            boost::radix::bits_type source_bits = get_bits_natural(packed, bit, Bits);
            BOOST_TEST(source_bits == unpacked[j]);
        }
    }
}

BOOST_AUTO_TEST_CASE(read_bytes)
{
    check_bits<1>();
    check_bits<2>();
    check_bits<3>();
    check_bits<4>();
    check_bits<5>();
    check_bits<6>();
    check_bits<7>();
}