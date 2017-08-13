//
// test/round_trip.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE TestRoundTrip
#include <boost/test/unit_test.hpp>

#include <boost/radix/decode.hpp>
#include <boost/radix/encode.hpp>
#include "generate_bytes.hpp"
#include "base64_reference.hpp"
#include <vector>
#include <algorithm>

#include <iostream>

BOOST_AUTO_TEST_CASE(round_trip_base64)
{
    boost::radix::alphabet base64("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
  
    std::vector<char> binary_in = generate_bytes(1024);
    std::string text;
    text.resize(boost::radix::encoded_size(binary_in.size(), base64));
    
    boost::radix::encode(binary_in.begin(), binary_in.end(), text.begin(), base64);

    std::vector<char> binary_out(boost::radix::decoded_size(text.size(), base64));
    boost::radix::decode(text.begin(), text.end(), binary_out.begin(), base64);
    BOOST_TEST(binary_in == binary_out);

    std::string ref_encode = base64::encode(reinterpret_cast<unsigned char*>(binary_in.data()), (int)binary_in.size());
    std::string ref_decode = base64::decode(ref_encode);

    std::cout << ref_encode << std::endl;
    std::cout << text;
}
