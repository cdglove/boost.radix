//
// test/ibstream.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE TestInBitstream
#include <boost/test/unit_test.hpp>

#include <boost/radix/detail/ibitstream.hpp>
#include "generate_bytes.hpp"
#include <vector>

BOOST_AUTO_TEST_CASE(read_bytes)
{
    std::vector<char> buf = generate_bytes(3 * 8 * 2);
    boost::radix::detail::ibitstream<std::vector<char>::iterator> s(buf.begin(), buf.end());
    BOOST_TEST(!s.finished());
    for(int i = 0; i < buf.size() * 8; i+=3)
    {
        char val = 0;
        s.read_bits(val, 3);
        BOOST_TEST(val == get_bits<char>(buf, i, 3));
    }

    BOOST_TEST(s.finished());
}

BOOST_AUTO_TEST_CASE(read_ints)
{
    std::vector<char> buf = generate_bytes(11 * 8 * 2);
    boost::radix::detail::ibitstream<std::vector<char>::iterator> s(buf.begin(), buf.end());
    BOOST_TEST(!s.finished());
    for(int i = 0; i < buf.size() * 8; i+=11)
    {
        int val = 0;
        s.read_bits(val, 11);
        BOOST_TEST(val == get_bits<int>(buf, i, 11));
    }
    BOOST_TEST(s.finished());
}