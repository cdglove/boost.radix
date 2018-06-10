//
// test/codec/rfc4648.cpp
//
// Copyright (c) Chris Glover, 2017-2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "../test/common.hpp"
#include "benchmark/benchmark.h"
#include <boost/beast/core/detail/base64.hpp>

template <typename Iterator>
auto unwrap_iterator(Iterator i) {
  return boost::addressof(*i);
}

static void Base64_Encode_Beast(benchmark::State& state) {
  std::vector<bits_type> data = generate_random_bytes(state.range(0));

  std::string result;
  result.resize(boost::beast::detail::base64::encoded_size(data.size()));
  for(auto _ : state) {
    boost::beast::detail::base64::encode(
        unwrap_iterator(result.begin()), data.data(), data.size());
    benchmark::DoNotOptimize(result);
  }

  state.SetBytesProcessed(
      int64_t(state.iterations()) * int64_t(state.range(0)));
}
BENCHMARK(Base64_Encode_Beast)
    ->Arg(128)
    ->Arg(1024)
    ->Arg(8 * 1024)
    ->Arg(64 * 1024)
    ->Arg(1024 * 1024);

static void Base64_Decode_Beast(benchmark::State& state) {
  std::vector<bits_type> data = generate_random_bytes(state.range(0));
  std::string encoded;
  encoded.resize(boost::beast::detail::base64::encoded_size(data.size()));
  boost::beast::detail::base64::encode(
      unwrap_iterator(encoded.begin()), data.data(), data.size());

  std::string result;
  result.resize(boost::beast::detail::base64::decoded_size(encoded.size()));
  for(auto _ : state) {
    boost::beast::detail::base64::decode(
        unwrap_iterator(result.begin()), encoded.data(), encoded.size());
    benchmark::DoNotOptimize(result);
  }

  state.SetBytesProcessed(
      int64_t(state.iterations()) * int64_t(state.range(0)));
}
BENCHMARK(Base64_Decode_Beast)
    ->Arg(128)
    ->Arg(1024)
    ->Arg(8 * 1024)
    ->Arg(64 * 1024)
    ->Arg(1024 * 1024);

BENCHMARK_MAIN();
