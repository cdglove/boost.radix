//
// test/codec/rfc4648.cpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "benchmark/benchmark.h"

#include <boost/radix/codec/rfc4648/base16.hpp>
#include <boost/radix/codec/rfc4648/base32.hpp>
#include <boost/radix/codec/rfc4648/base32hex.hpp>
#include <boost/radix/codec/rfc4648/base64.hpp>
#include <boost/radix/codec/rfc4648/base64url.hpp>
#include <boost/radix/decode.hpp>
#include <boost/radix/encode.hpp>

#include "../test/common.hpp"

#include <boost/radix/static_ibitstream_lsb.hpp>
#include <boost/radix/static_obitstream_lsb.hpp>

struct base64_lsb : boost::radix::codec::rfc4648::base64
{};

boost::radix::static_obitstream_lsb<
    boost::radix::required_bits<base64_lsb>::value>
get_segment_packer(base64_lsb const&)
{
    return boost::radix::static_obitstream_lsb<
        boost::radix::required_bits<base64_lsb>::value>();
}

boost::radix::static_ibitstream_lsb<
    boost::radix::required_bits<base64_lsb>::value>
get_segment_unpacker(base64_lsb const&)
{
    return boost::radix::static_ibitstream_lsb<
        boost::radix::required_bits<base64_lsb>::value>();
}

//static void Base64_Encode_BackInserter(benchmark::State& state)
//{
//    boost::radix::codec::rfc4648::base64 codec;
//
//    std::vector<bits_type> data = generate_random_bytes(state.range(0));
//
//    for(auto _ : state)
//    {
//        std::string result;
//        boost::radix::encode(
//            data.begin(), data.end(), std::back_inserter(result), codec);
//    }
//
//    state.SetBytesProcessed(
//        int64_t(state.iterations()) * int64_t(state.range(0)));
//}
//BENCHMARK(Base64_Encode_BackInserter)
//    ->Arg(128)
//    ->Arg(1024)
//    ->Arg(8 * 1024)
//    ->Arg(64 * 1024)
//    ->Arg(1024 * 1024);
//
//static void Base64_Decode_BackInserter(benchmark::State& state)
//{
//    boost::radix::codec::rfc4648::base64 codec;
//
//    std::vector<bits_type> data = generate_random_bytes(state.range(0));
//
//    std::string encoded;
//    boost::radix::encode(
//        data.begin(), data.end(), std::back_inserter(encoded), codec);
//
//    for(auto _ : state)
//    {
//        std::vector<bits_type> result;
//        boost::radix::decode(
//            encoded.begin(), encoded.end(), std::back_inserter(result), codec);
//    }
//
//    state.SetBytesProcessed(
//        int64_t(state.iterations()) * int64_t(state.range(0)));
//}
//BENCHMARK(Base64_Decode_BackInserter)
//    ->Arg(128)
//    ->Arg(1024)
//    ->Arg(8 * 1024)
//    ->Arg(64 * 1024)
//    ->Arg(1024 * 1024);

static void Base64_Encode_OutputDirect(benchmark::State& state)
{
    boost::radix::codec::rfc4648::base64 codec;

    std::vector<bits_type> data = generate_random_bytes(state.range(0));

    std::string result;
    result.resize(encoded_size(data.size(), codec));
    for(auto _ : state)
    {
        boost::radix::encode(data.begin(), data.end(), result.begin(), codec);
    }

    state.SetBytesProcessed(
        int64_t(state.iterations()) * int64_t(state.range(0)));
}
BENCHMARK(Base64_Encode_OutputDirect)
    ->Arg(128)
    ->Arg(1024)
    ->Arg(8 * 1024)
    ->Arg(64 * 1024)
    ->Arg(1024 * 1024);

static void Base64_Lsb_Encode_OutputDirect(benchmark::State& state)
{
    base64_lsb codec;

    std::vector<bits_type> data = generate_random_bytes(state.range(0));

    std::string result;
    result.resize(encoded_size(data.size(), codec));
    for(auto _ : state)
    {
        boost::radix::encode(data.begin(), data.end(), result.begin(), codec);
    }

    state.SetBytesProcessed(
        int64_t(state.iterations()) * int64_t(state.range(0)));
}
BENCHMARK(Base64_Lsb_Encode_OutputDirect)
    ->Arg(128)
    ->Arg(1024)
    ->Arg(8 * 1024)
    ->Arg(64 * 1024)
    ->Arg(1024 * 1024);

//static void Base64_Decode_OutputDirect(benchmark::State& state)
//{
//    boost::radix::codec::rfc4648::base64 codec;
//
//    std::vector<bits_type> data = generate_random_bytes(state.range(0));
//
//    std::string encoded;
//    boost::radix::encode(
//        data.begin(), data.end(), std::back_inserter(encoded), codec);
//
//    std::vector<bits_type> result(data.size());
//    for(auto _ : state)
//    {
//        boost::radix::decode(
//            encoded.begin(), encoded.end(), result.begin(), codec);
//    }
//
//    state.SetBytesProcessed(
//        int64_t(state.iterations()) * int64_t(state.range(0)));
//}
//BENCHMARK(Base64_Decode_OutputDirect)
//    ->Arg(128)
//    ->Arg(1024)
//    ->Arg(8 * 1024)
//    ->Arg(64 * 1024)
//    ->Arg(1024 * 1024);

//static void Base64_Lsb_Decode_OutputDirect(benchmark::State& state)
//{
//    base64_lsb codec;
//
//    std::vector<bits_type> data = generate_random_bytes(state.range(0));
//
//    std::string encoded;
//    boost::radix::encode(
//        data.begin(), data.end(), std::back_inserter(encoded), codec);
//
//    std::vector<bits_type> result(data.size());
//    for(auto _ : state)
//    {
//        boost::radix::decode(
//            encoded.begin(), encoded.end(), result.begin(), codec);
//    }
//
//    state.SetBytesProcessed(
//        int64_t(state.iterations()) * int64_t(state.range(0)));
//}
//BENCHMARK(Base64_Lsb_Decode_OutputDirect)
//    ->Arg(128)
//    ->Arg(1024)
//    ->Arg(8 * 1024)
//    ->Arg(64 * 1024)
//    ->Arg(1024 * 1024);

BENCHMARK_MAIN();