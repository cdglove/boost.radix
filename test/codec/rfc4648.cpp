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

#include <boost/radix/codec/rfc4648/base16.hpp>
#include <boost/radix/codec/rfc4648/base32.hpp>
#include <boost/radix/codec/rfc4648/base32hex.hpp>
#include <boost/radix/codec/rfc4648/base64.hpp>
#include <boost/radix/codec/rfc4648/base64url.hpp>
#include <boost/radix/decode.hpp>
#include <boost/radix/encode.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

// Test vectors taken from https://tools.ietf.org/html/rfc4648

#include "../test/base64_simd.hpp"

// -----------------------------------------------------------------------------
//
template <typename CharType, typename Codec>
std::basic_string<boost::radix::char_type> encode_string(
    boost::basic_string_view<CharType> input, Codec const& codec) {
  using boost::radix::adl::get_segment_unpacker;
  std::string result;
  result.resize(encoded_size(
      std::distance(boost::begin(input), boost::end(input)), codec));

  std::size_t encoded_size = boost::beast::detail::base64::encode_simd(
      (void*)result.data(), input.data(), input.length());
  result.resize(encoded_size);
  return result;
}

// -----------------------------------------------------------------------------
//
template <typename Codec>
std::basic_string<boost::radix::char_type> encode_string(
    char const* input, Codec const& codec) {
  return encode_string(boost::string_view(input), codec);
}

// -----------------------------------------------------------------------------
//
template <typename CharType, typename Codec>
std::basic_string<boost::radix::char_type> decode_string(
    boost::basic_string_view<CharType> input, Codec const& codec) {
  using boost::radix::adl::get_segment_unpacker;
  std::string result;
  result.resize(decoded_size(
      std::distance(boost::begin(input), boost::end(input)), codec));

  std::size_t decoded_size =
      decode(boost::begin(input), boost::end(input), result.begin(), codec);
  result.resize(decoded_size);
  return result;
}

// -----------------------------------------------------------------------------
//
template <typename Codec>
std::basic_string<boost::radix::char_type> decode_string(
    char const* input, Codec const& codec) {
  return decode_string(boost::string_view(input), codec);
}

BOOST_AUTO_TEST_CASE(base64) {
  boost::radix::codec::rfc4648::base64 codec;

  BOOST_TEST(encode_string("", codec) == "");
  BOOST_TEST(encode_string("f", codec) == "Zg==");
  BOOST_TEST(encode_string("fo", codec) == "Zm8=");
  BOOST_TEST(encode_string("foo", codec) == "Zm9v");
  BOOST_TEST(encode_string("foob", codec) == "Zm9vYg==");
  BOOST_TEST(encode_string("fooba", codec) == "Zm9vYmE=");
  BOOST_TEST(encode_string("foobar", codec) == "Zm9vYmFy");
  BOOST_TEST(
      encode_string(
          "The particular choice of characters to make up the 64 characters "
          "required for base varies between implementations. The general rule "
          "is to choose a set of 64 characters that is both part of a subset "
          "common to most encodings, and also printable. This combination "
          "leaves the data unlikely to be modified in transit through systems, "
          "such as email, which were traditionally not 8-bit clean. For "
          "example, MIME's Base64 implementation uses A-Z, a-z, and 0-9 for "
          "the first 62 values. Other variations, usually derived from Base64, "
          "share this property but differ in the symbols chosen for the last "
          "two values; an example is UTF-7.",
          codec) ==
      "VGhlIHBhcnRpY3VsYXIgY2hvaWNlIG9mIGNoYXJhY3RlcnMgdG8gbWFrZSB1cCB0aGUgNjQg"
      "Y2hhcmFjdGVycyByZXF1aXJlZCBmb3IgYmFzZSB2YXJpZXMgYmV0d2VlbiBpbXBsZW1lbnRh"
      "dGlvbnMuIFRoZSBnZW5lcmFsIHJ1bGUgaXMgdG8gY2hvb3NlIGEgc2V0IG9mIDY0IGNoYXJh"
      "Y3RlcnMgdGhhdCBpcyBib3RoIHBhcnQgb2YgYSBzdWJzZXQgY29tbW9uIHRvIG1vc3QgZW5j"
      "b2RpbmdzLCBhbmQgYWxzbyBwcmludGFibGUuIFRoaXMgY29tYmluYXRpb24gbGVhdmVzIHRo"
      "ZSBkYXRhIHVubGlrZWx5IHRvIGJlIG1vZGlmaWVkIGluIHRyYW5zaXQgdGhyb3VnaCBzeXN0"
      "ZW1zLCBzdWNoIGFzIGVtYWlsLCB3aGljaCB3ZXJlIHRyYWRpdGlvbmFsbHkgbm90IDgtYml0"
      "IGNsZWFuLiBGb3IgZXhhbXBsZSwgTUlNRSdzIEJhc2U2NCBpbXBsZW1lbnRhdGlvbiB1c2Vz"
      "IEEtWiwgYS16LCBhbmQgMC05IGZvciB0aGUgZmlyc3QgNjIgdmFsdWVzLiBPdGhlciB2YXJp"
      "YXRpb25zLCB1c3VhbGx5IGRlcml2ZWQgZnJvbSBCYXNlNjQsIHNoYXJlIHRoaXMgcHJvcGVy"
      "dHkgYnV0IGRpZmZlciBpbiB0aGUgc3ltYm9scyBjaG9zZW4gZm9yIHRoZSBsYXN0IHR3byB2"
      "YWx1ZXM7IGFuIGV4YW1wbGUgaXMgVVRGLTcu");

  BOOST_TEST(decode_string("", codec) == "");
  BOOST_TEST(decode_string("Zg==", codec) == "f");
  BOOST_TEST(decode_string("Zm8=", codec) == "fo");
  BOOST_TEST(decode_string("Zm9v", codec) == "foo");
  BOOST_TEST(decode_string("Zm9vYg==", codec) == "foob");
  BOOST_TEST(decode_string("Zm9vYmE=", codec) == "fooba");
  BOOST_TEST(decode_string("Zm9vYmFy", codec) == "foobar");
}

BOOST_AUTO_TEST_CASE(base64url) {
  boost::radix::codec::rfc4648::base64url codec;

  BOOST_TEST(encode_string("", codec) == "");
  BOOST_TEST(encode_string("f", codec) == "Zg==");
  BOOST_TEST(encode_string("fo", codec) == "Zm8=");
  BOOST_TEST(encode_string("foo", codec) == "Zm9v");
  BOOST_TEST(encode_string("foob", codec) == "Zm9vYg==");
  BOOST_TEST(encode_string("fooba", codec) == "Zm9vYmE=");
  BOOST_TEST(encode_string("foobar", codec) == "Zm9vYmFy");

  BOOST_TEST(decode_string("", codec) == "");
  BOOST_TEST(decode_string("Zg==", codec) == "f");
  BOOST_TEST(decode_string("Zm8=", codec) == "fo");
  BOOST_TEST(decode_string("Zm9v", codec) == "foo");
  BOOST_TEST(decode_string("Zm9vYg==", codec) == "foob");
  BOOST_TEST(decode_string("Zm9vYmE=", codec) == "fooba");
  BOOST_TEST(decode_string("Zm9vYmFy", codec) == "foobar");
}

// BOOST_AUTO_TEST_CASE(base32) {
//  boost::radix::codec::rfc4648::base32 codec;
//
//  BOOST_TEST(encode_string("", codec) == "");
//  BOOST_TEST(encode_string("f", codec) == "MY======");
//  BOOST_TEST(encode_string("fo", codec) == "MZXQ====");
//  BOOST_TEST(encode_string("foo", codec) == "MZXW6===");
//  BOOST_TEST(encode_string("foob", codec) == "MZXW6YQ=");
//  BOOST_TEST(encode_string("fooba", codec) == "MZXW6YTB");
//  BOOST_TEST(encode_string("foobar", codec) == "MZXW6YTBOI======");
//
//  BOOST_TEST(decode_string("", codec) == "");
//  BOOST_TEST(decode_string("MY======", codec) == "f");
//  BOOST_TEST(decode_string("MZXQ====", codec) == "fo");
//  BOOST_TEST(decode_string("MZXW6===", codec) == "foo");
//  BOOST_TEST(decode_string("MZXW6YQ=", codec) == "foob");
//  BOOST_TEST(decode_string("MZXW6YTB", codec) == "fooba");
//  BOOST_TEST(decode_string("MZXW6YTBOI======", codec) == "foobar");
//}
//
// BOOST_AUTO_TEST_CASE(base32hex) {
//  boost::radix::codec::rfc4648::base32hex codec;
//
//  BOOST_TEST(encode_string("", codec) == "");
//  BOOST_TEST(encode_string("f", codec) == "CO======");
//  BOOST_TEST(encode_string("fo", codec) == "CPNG====");
//  BOOST_TEST(encode_string("foo", codec) == "CPNMU===");
//  BOOST_TEST(encode_string("foob", codec) == "CPNMUOG=");
//  BOOST_TEST(encode_string("fooba", codec) == "CPNMUOJ1");
//  BOOST_TEST(encode_string("foobar", codec) == "CPNMUOJ1E8======");
//
//  BOOST_TEST(decode_string("", codec) == "");
//  BOOST_TEST(decode_string("CO======", codec) == "f");
//  BOOST_TEST(decode_string("CPNG====", codec) == "fo");
//  BOOST_TEST(decode_string("CPNMU===", codec) == "foo");
//  BOOST_TEST(decode_string("CPNMUOG=", codec) == "foob");
//  BOOST_TEST(decode_string("CPNMUOJ1", codec) == "fooba");
//  BOOST_TEST(decode_string("CPNMUOJ1E8======", codec) == "foobar");
//}
//
// BOOST_AUTO_TEST_CASE(base16) {
//  boost::radix::codec::rfc4648::base16 codec;
//
//  BOOST_TEST(encode_string("", codec) == "");
//  BOOST_TEST(encode_string("f", codec) == "66");
//  BOOST_TEST(encode_string("fo", codec) == "666F");
//  BOOST_TEST(encode_string("foo", codec) == "666F6F");
//  BOOST_TEST(encode_string("foob", codec) == "666F6F62");
//  BOOST_TEST(encode_string("fooba", codec) == "666F6F6261");
//  BOOST_TEST(encode_string("foobar", codec) == "666F6F626172");
//
//  BOOST_TEST(decode_string("", codec) == "");
//  BOOST_TEST(decode_string("66", codec) == "f");
//  BOOST_TEST(decode_string("666F", codec) == "fo");
//  BOOST_TEST(decode_string("666F6F", codec) == "foo");
//  BOOST_TEST(decode_string("666F6F62", codec) == "foob");
//  BOOST_TEST(decode_string("666F6F6261", codec) == "fooba");
//  BOOST_TEST(decode_string("666F6F626172", codec) == "foobar");
//}
