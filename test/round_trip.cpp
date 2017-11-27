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
#include <boost/radix/codec.hpp>
#include <boost/range/algorithm/equal.hpp>
#include "generate_bytes.hpp"
#include "base64_reference.hpp"
#include <vector>
#include <algorithm>

#include <iostream>

BOOST_AUTO_TEST_CASE(round_trip_base64)
{
    boost::radix::codec<64> base64(boost::radix::alphabet<64>("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"), '=');
  
    std::vector<char> binary_in = generate_bytes(1024);
    
    std::string encoded_text;
    boost::radix::encode(binary_in.begin(), binary_in.end(), std::back_inserter(encoded_text), base64);

    std::vector<char> decoded_binary;
    boost::radix::decode(encoded_text.begin(), encoded_text.end(), std::back_inserter(decoded_binary), base64);
    
    BOOST_TEST(binary_in == decoded_binary);

    std::string ref_encode = base64::encode(reinterpret_cast<unsigned char*>(binary_in.data()), (int)binary_in.size());
    std::string ref_decode = base64::decode(ref_encode);

    BOOST_TEST(ref_encode == encoded_text);
    BOOST_TEST(boost::equal(ref_decode, decoded_binary));
}
