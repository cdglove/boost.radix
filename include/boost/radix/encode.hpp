//
// boost/radix/encode.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_ENCODE_HPP
#define BOOST_RADIX_ENCODE_HPP

#include <boost/radix/common.hpp>

#include <boost/radix/codec_traits/pad.hpp>
#include <boost/radix/codec_traits/segment.hpp>
#include <boost/radix/codec_traits/whitespace.hpp>
#include <boost/radix/static_ibitstream_msb.hpp>

#include <boost/array.hpp>
#include <boost/move/utility.hpp>

#include <memory>
#include <utility>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif

// -----------------------------------------------------------------------------
//
namespace boost { namespace radix {

// -----------------------------------------------------------------------------
//
namespace adl {

template <typename Codec>
std::size_t get_encoded_size(std::size_t source_size, Codec const& codec) {
  // By default, size is an integer multiple of the output
  // segment size.
  return codec_traits::unpacked_segment_size<Codec>::value *
         (source_size + (codec_traits::packed_segment_size<Codec>::value - 1) /
                            codec_traits::packed_segment_size<Codec>::value);
}

template <typename Codec>
static_ibitstream_msb<codec_traits::required_bits<Codec>::value>
get_segment_unpacker(Codec const& codec) {
  return static_ibitstream_msb<codec_traits::required_bits<Codec>::value>();
}

} // namespace adl

// -----------------------------------------------------------------------------
//
namespace detail {

template <typename Codec>
struct bits_to_char_mapper {
  bits_to_char_mapper(Codec const& codec)
      : codec_(codec) {
  }

  char_type operator()(bits_type bits) const {
    return codec_.char_from_bits(bits);
  }

  Codec const& codec_;
};

// -----------------------------------------------------------------------------
//
template <typename InnerIterator, std::size_t MaxLineLength>
class line_break_iterator
    : public std::iterator<std::output_iterator_tag, void, void, void, void> {
 public:
  line_break_iterator(InnerIterator iter, std::size_t current_char)
      : iter_(iter)
      , char_(current_char) {
  }

  line_break_iterator& operator++() {
    return *this;
  }

  line_break_iterator& operator++(int) {
    return *this;
  }

  line_break_iterator& operator*() {
    return *this;
  }

  template <typename T>
  line_break_iterator& operator=(T const& t) {
    ++char_;
    if(char_ == MaxLineLength) {
      *iter_++ = '\n';
      char_    = 0;
    }

    *iter_++ = t;
    return *this;
  }

  operator InnerIterator() {
    return iter_;
  }

 private:
  InnerIterator iter_;
  std::size_t char_;
};

template <typename Codec, typename OutputIterator>
line_break_iterator<
    OutputIterator,
    codec_traits::max_encoded_line_length<Codec>::value>
maybe_add_line_break_iterator(
    Codec const&, OutputIterator out, boost::true_type) {
  return line_break_iterator<
      OutputIterator, codec_traits::max_encoded_line_length<Codec>::value>(out);
}

template <typename Codec, typename OutputIterator>
OutputIterator maybe_add_line_break_iterator(
    Codec const&, OutputIterator out, boost::false_type) {
  return out;
}

// -----------------------------------------------------------------------------
//
template <typename Iterator, typename EndIterator, typename PackedSegment>
bool fill_packed_segment(
    Iterator& first, EndIterator last, PackedSegment& packed) {
  typename PackedSegment::iterator pbegin = packed.end();
  typename PackedSegment::iterator pend   = packed.begin() + packed.capacity();

  while(first != last && pbegin != pend) {
    *pbegin++ = *first++;
  }

  packed.resize(std::distance(packed.begin(), pbegin));
  return pbegin == pend;
}

template <typename Codec>
std::size_t get_unpacked_size_from_packed_size(
    Codec const&, std::size_t packed_size) {
  std::size_t bits_written = packed_size * 8;
  std::size_t bytes_written =
      (bits_written + (codec_traits::required_bits<Codec>::value - 1)) /
      codec_traits::required_bits<Codec>::value;
  return bytes_written;
}

template <typename Codec, typename UnpackedBuffer>
std::size_t maybe_pad_segment_impl(
    Codec const& codec,
    std::size_t packed_size,
    UnpackedBuffer& unpacked,
    boost::true_type) {
  std::size_t bytes_written =
      get_unpacked_size_from_packed_size(codec, packed_size);

  while(bytes_written < unpacked.size())
    unpacked[bytes_written++] = codec.get_pad_bits();

  return unpacked.size();
}

template <typename Codec, typename UnpackedBuffer>
std::size_t maybe_pad_segment_impl(
    Codec const& codec,
    std::size_t packed_size,
    UnpackedBuffer& unpacked,
    boost::false_type) {
  return get_unpacked_size_from_packed_size(codec, packed_size);
}

template <typename Codec, typename UnpackedBuffer>
std::size_t maybe_pad_segment(
    Codec const& codec, std::size_t packed_size, UnpackedBuffer& unpacked) {
  return maybe_pad_segment_impl(
      codec, packed_size, unpacked,
      typename codec_traits::requires_pad<Codec>::type());
}

// -----------------------------------------------------------------------------
//
template <typename Codec, typename InputIterator, typename OutputIterator>
OutputIterator format_segment(
    Codec const& codec,
    InputIterator first,
    InputIterator last,
    OutputIterator to) {
  return std::transform(
      first, last,
      maybe_add_line_break_iterator(
          codec, to,
          typename codec_traits::requires_line_breaks<Codec>::type()),
      detail::bits_to_char_mapper<Codec>(codec));
}

template <
    typename Codec,
    typename RandomAccessInputIterator,
    typename OutputIterator,
    typename SegmentUnpacker>
OutputIterator unpack_segment(
    Codec const& codec,
    RandomAccessInputIterator from,
    OutputIterator to,
    SegmentUnpacker segment_unpacker) {
  typedef boost::array<
      bits_type, codec_traits::unpacked_segment_size<Codec>::value>
      buffer_type;
  buffer_type buffer;
  //typename buffer_type::iterator buf = buffer.begin();
  segment_unpacker(from, buffer);
  return format_segment(codec, buffer.begin(), buffer.end(), to);
}

} // namespace detail

// -----------------------------------------------------------------------------
//
template <typename Codec, typename OutputIterator>
class encoder {
 public:
  typedef Codec codec_type;
  typedef OutputIterator iterator_type;

  encoder(Codec const& codec, OutputIterator out)
      : codec_(codec)
      , out_(out)
      , bytes_written_(0) {
  }

  ~encoder() {
    resolve();
  }

  template <typename Iterator, typename EndIterator>
  std::size_t append(Iterator first, EndIterator last) {
    using boost::radix::adl::get_segment_unpacker;
    std::size_t bytes_appended =
        append_impl(first, last, get_segment_unpacker(codec_));
    bytes_written_ += bytes_appended;
    return bytes_appended;
  }

  std::size_t append(bits_type bits) {
    packed_segment_.push_back(bits);
    if(packed_segment_.full()) {
      using boost::radix::adl::get_segment_unpacker;
      typename packed_segment_type::iterator in = packed_segment_.begin();
      out_ = ::boost::radix::detail::unpack_segment(
          codec_, in, out_, get_segment_unpacker(codec_));
      bytes_written_ += codec_traits::unpacked_segment_size<Codec>::value;
      packed_segment_.clear();
    }
    return 1;
  }

  std::size_t resolve() {
    if(packed_segment_.empty())
      return 0;

    std::fill(
        packed_segment_.end(),
        packed_segment_.begin() + packed_segment_.capacity(), 0);

    typedef boost::array<
        char_type, codec_traits::unpacked_segment_size<Codec>::value>
        unpacked_segment_type;
    unpacked_segment_type unpacked_segment;

    using boost::radix::adl::get_segment_unpacker;
    typename packed_segment_type::iterator in    = packed_segment_.begin();
    typename unpacked_segment_type::iterator out = unpacked_segment.begin();
    get_segment_unpacker(codec_)(in, out);

    std::size_t unpacked_size = ::boost::radix::detail::maybe_pad_segment(
        codec_, packed_segment_.size(), unpacked_segment);
    packed_segment_.clear();

    out_ = std::transform(
        unpacked_segment.begin(), unpacked_segment.begin() + unpacked_size,
        out_, detail::bits_to_char_mapper<Codec>(codec_));

    bytes_written_ += unpacked_size;

    return unpacked_size;
  }

  void abort() {
    packed_segment_.clear();
  }

  void reset(OutputIterator out) {
    abort();
    bytes_written_ = 0;
    out_           = boost::move(out);
  }

  std::size_t bytes_written() const {
    return bytes_written_;
  }

 private:
  //
  template <typename Iterator, typename EndIterator, typename SegmentUnpacker>
  std::size_t append_impl(
      Iterator first, EndIterator last, SegmentUnpacker segment_unpacker) {
    std::size_t bytes_appended = 0;
    if(!packed_segment_.empty()) {
      if(!fill_packed_segment(first, last, packed_segment_)) {
        return 0;
      }
      BOOST_ASSERT(packed_segment_.full());
      typename packed_segment_type::iterator in = packed_segment_.begin();
      out_ = ::boost::radix::detail::unpack_segment(
          codec_, in, out_, segment_unpacker);
      packed_segment_.clear();
      bytes_appended += codec_traits::unpacked_segment_size<Codec>::value;
    }

    bytes_appended += direct_write_segments(
        first, last, segment_unpacker,
        typename std::iterator_traits<Iterator>::iterator_category());
    return bytes_appended;
  }

  template <typename Iterator, typename SegmentUnpacker>
  std::size_t direct_write_segments(
      Iterator first,
      Iterator last,
      SegmentUnpacker& segment_unpacker,
      std::random_access_iterator_tag) {
    std::size_t bytes_appended = 0;
    while(std::distance(first, last) >=
          codec_traits::packed_segment_size<Codec>::value) {
      out_ = ::boost::radix::detail::unpack_segment(
          codec_, first, out_, segment_unpacker);
      first += codec_traits::packed_segment_size<Codec>::value;
      bytes_appended += codec_traits::unpacked_segment_size<Codec>::value;
    }

    fill_packed_segment(first, last, packed_segment_);
    return bytes_appended;
  }

  template <typename Iterator, typename EndIterator, typename SegmentUnpacker>
  std::size_t direct_write_segments(
      Iterator first,
      EndIterator last,
      SegmentUnpacker& segment_unpacker,
      ...) {
    std::size_t bytes_appended = 0;
    while(true) {
      if(!fill_packed_segment(first, last, packed_segment_))
        break;
      typename packed_segment_type::iterator in = packed_segment_.begin();
      out_ = ::boost::radix::detail::unpack_segment(
          codec_, in, out_, segment_unpacker);
      packed_segment_.clear();
      bytes_appended += codec_traits::unpacked_segment_size<Codec>::value;
    }

    return bytes_appended;
  }

  Codec const& codec_;
  OutputIterator out_;
  std::size_t bytes_written_;

  typedef detail::
      segment_buffer<bits_type, codec_traits::packed_segment_size<Codec>::value>
          packed_segment_type;
  packed_segment_type packed_segment_;
}; // namespace radix

template <typename Codec, typename OutputIterator>
encoder<Codec, OutputIterator> make_encoder(
    Codec const& codec, OutputIterator out) {
  return encoder<Codec, OutputIterator>(codec, out);
}

// -----------------------------------------------------------------------------
//
template <typename Codec>
std::size_t encoded_size(std::size_t source_size, Codec const& codec) {
  using boost::radix::adl::get_encoded_size;
  return get_encoded_size(source_size, codec);
}

// -----------------------------------------------------------------------------
//
template <
    typename InputIterator,
    typename InputEndIterator,
    typename OutputIterator,
    typename Codec>
std::size_t encode(
    InputIterator first,
    InputEndIterator last,
    OutputIterator out,
    Codec const& codec) {
  encoder<Codec, OutputIterator> e(codec, out);
  e.append(first, last);
  e.resolve();
  return e.bytes_written();
}

}} // namespace boost::radix

#endif // BOOST_RADIX_ENCODE_HPP
