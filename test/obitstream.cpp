//
// test/ibstream.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE TestOutBitstream
#include <boost/test/unit_test.hpp>

#include <boost/radix/detail/obitstream.hpp>
#include "generate_bytes.hpp"
#include <vector>



BOOST_AUTO_TEST_CASE(write_bytes)
{
    std::vector<char> in_buf = generate_bytes(3 * 8 * 2);
    std::vector<char> out_buf;
    out_buf.resize(in_buf.size());
    boost::radix::detail::obitstream<std::vector<char>::iterator> s(out_buf.begin());
    for(int i = 0; i < in_buf.size() * 8; i+=3)
    {
        s.write_bits(get_bits<char>(in_buf, i, 3), 3);
    }

    BOOST_TEST(in_buf == out_buf);
}

BOOST_AUTO_TEST_CASE(write_ints)
{
    std::vector<char> in_buf = generate_bytes(11 * 8 * 2);
    std::vector<char> out_buf;
    out_buf.resize(in_buf.size());
    boost::radix::detail::obitstream<std::vector<char>::iterator> s(out_buf.begin());
    for(int i = 0; i < in_buf.size() * 8; i+=11)
    {
        s.write_bits(get_bits<int>(in_buf, i, 11), 11);
    }

    BOOST_TEST(in_buf == out_buf);
}