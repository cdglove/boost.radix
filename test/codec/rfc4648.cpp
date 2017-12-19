//
// test/codec/rfc4648.cpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE TestRfc4648
#include <boost/test/unit_test.hpp>

#include <boost/radix/codec/rfc4648/base64.hpp>
#include <boost/radix/codec/rfc4648/base64url.hpp>
#include <boost/radix/codec/rfc4648/base32.hpp>
#include <boost/radix/codec/rfc4648/base32hex.hpp>
#include <boost/radix/codec/rfc4648/base16.hpp>
#include <boost/radix/decode.hpp>
#include <boost/radix/encode.hpp>

// Test vectors taken from https://tools.ietf.org/html/rfc4648

BOOST_AUTO_TEST_CASE(base64)
{
	// std::string result;
	// boost::radix::encode(
}

BOOST_AUTO_TEST_CASE(base64url)
{

}

BOOST_AUTO_TEST_CASE(base32)
{

}

BOOST_AUTO_TEST_CASE(base32hex)
{

}

BOOST_AUTO_TEST_CASE(base16)
{

}