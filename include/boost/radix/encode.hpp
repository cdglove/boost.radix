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
template <typename InnerIterator>
class passthrough_output_iterator
    : public std::iterator<std::output_iterator_tag, void, void, void, void> {
 public:
  passthrough_output_iterator(InnerIterator iter)
      : iter_(iter) {
  }

  passthrough_output_iterator& operator++() {
    return *this;
  }

  passthrough_output_iterator& operator++(int) {
    return *this;
  }

  passthrough_output_iterator& operator*() {
    return *this;
  }

  template <typename T>
  passthrough_output_iterator& operator=(T const& t) {
    *(iter_)++ = t;
    return *this;
  }

  InnerIterator unwrap() const {
    return iter_;
  }

 private:
  InnerIterator iter_;
};

// -----------------------------------------------------------------------------
//
template <typename InnerIterator>
class reference_output_iterator
    : public std::iterator<std::output_iterator_tag, void, void, void, void> {
 public:
  reference_output_iterator(InnerIterator* iter)
      : iter_(iter) {
  }

  reference_output_iterator& operator++() {
    return *this;
  }

  reference_output_iterator& operator++(int) {
    return *this;
  }

  reference_output_iterator& operator*() {
    return *this;
  }

  template <typename T>
  reference_output_iterator& operator=(T const& t) {
    *(*iter_)++ = t;
    return *this;
  }

  InnerIterator unwrap() const {
    return *iter_;
  }

 private:
  InnerIterator* iter_;
};

template <typename Iterator>
reference_output_iterator<Iterator> make_iterator_reference(Iterator& i) {
  return reference_output_iterator<Iterator>(&i);
}

// -----------------------------------------------------------------------------
//
template <typename InnerIterator, std::size_t MaxLineLength>
class line_break_iterator
    : public std::iterator<std::output_iterator_tag, void, void, void, void> {
 public:
  line_break_iterator(InnerIterator iter)
      : iter_(iter)
      , char_(0) {
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
      *(iter_)++ = '\n';
      char_      = 0;
    }

    *(iter_)++ = t;
    return *this;
  }

  InnerIterator unwrap() const {
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
passthrough_output_iterator<OutputIterator> maybe_add_line_break_iterator(
    Codec const&, OutputIterator out, boost::false_type) {
  return passthrough_output_iterator<OutputIterator>(out);
}

// -----------------------------------------------------------------------------
//
template <typename Codec, typename InnerIterator>
class char_from_bits_iterator
    : public std::iterator<std::output_iterator_tag, void, void, void, void> {
 public:
  char_from_bits_iterator(Codec const& codec, InnerIterator iter)
      : codec_(&codec)
      , iter_(iter) {
  }

  char_from_bits_iterator& operator++() {
    return *this;
  }

  char_from_bits_iterator& operator++(int) {
    return *this;
  }

  char_from_bits_iterator& operator*() {
    return *this;
  }

  char_from_bits_iterator& operator=(bits_type bits) {
    *(iter_)++ = codec_->char_from_bits(bits);
    return *this;
  }

  InnerIterator unwrap() const {
    return iter_;
  }

 private:
  Codec const* codec_;
  InnerIterator iter_;
};

template <typename Codec, typename OutputIterator>
char_from_bits_iterator<Codec, OutputIterator> make_char_from_bits_iterator(
    Codec const& codec, OutputIterator out) {
  return char_from_bits_iterator<Codec, OutputIterator>(codec, out);
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
static std::size_t maybe_pad_segment_impl(
    Codec const& codec,
    std::size_t packed_size,
    UnpackedBuffer& unpacked,
    boost::false_type) {
  return get_unpacked_size_from_packed_size(codec, packed_size);
}

template <typename Codec, typename UnpackedBuffer>
static std::size_t maybe_pad_segment(
    Codec const& codec, std::size_t packed_size, UnpackedBuffer& unpacked) {
  return maybe_pad_segment_impl(
      codec, packed_size, unpacked,
      typename codec_traits::requires_pad<Codec>::type());
}

template <typename InputIterator, typename OutputIterator>
void encode_full_segment(
    InputIterator& first, InputIterator last, OutputIterator out) {
}

template <
    typename InputIterator,
    typename InputEndIterator,
    typename OutputIterator>
void encode_partial_segment(
    InputIterator& first, InputIterator last, OutputIterator out) {
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
    return append_impl(first, last, get_segment_unpacker(codec_));
  }

  std::size_t append(bits_type bits) {
    packed_segment_.push_back(bits);
    if(packed_segment_.full()) {
      using boost::radix::adl::get_segment_unpacker;
      write_segment(packed_segment_.begin(), get_segment_unpacker(codec_));
      bytes_written_ += codec_traits::unpacked_segment_size<Codec>::value;
    }
    return 1;
  }

  std::size_t resolve() {
    if(packed_segment_.empty())
      return 0;

    std::fill(
        packed_segment_.end(),
        packed_segment_.begin() + packed_segment_.capacity(), 0);

    boost::array<char_type, codec_traits::unpacked_segment_size<Codec>::value>
        unpacked_segment;

    using boost::radix::adl::get_segment_unpacker;
    get_segment_unpacker(codec_)(
        packed_segment_.begin(), unpacked_segment.begin());

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
    out_           = std::move(out);
  }

  std::size_t bytes_written() const {
    return bytes_written_;
  }

 private:
  //
  template <typename Iterator, typename EndIterator, typename SegmentUnpacker>
  std::size_t append_impl(
      Iterator first, EndIterator last, SegmentUnpacker segment_unpacker) {

    if(!packed_segment_.empty() && !fill_packed_segment(first, last, packed_segment_))
      return 0;
    write_segment(packed_segment_.begin(), segment_unpacker);
    packed_segment_.clear();
    std::size_t bytes_append =
        codec_traits::unpacked_segment_size<Codec>::value;
    bytes_append += write_segments(
        first, last, segment_unpacker,
        typename std::iterator_traits<Iterator>::iterator_category());
    bytes_written_ += bytes_append;
    return bytes_append;
  }

  template <typename Iterator, typename SegmentUnpacker>
  void write_segment(Iterator first, SegmentUnpacker segment_unpacker) {
    segment_unpacker(
        first,
        detail::make_char_from_bits_iterator(
            codec_,
            detail::maybe_add_line_break_iterator(
                codec_, detail::make_iterator_reference(out_),
                typename codec_traits::requires_line_breaks<Codec>::type())));
  }

  template <typename Iterator, typename EndIterator, typename SegmentUnpacker>
  std::size_t write_segments(
      Iterator first,
      EndIterator last,
      SegmentUnpacker segment_unpacker,
      std::random_access_iterator_tag) {
    std::size_t bytes_append = 0;
    while(std::distance(first, last) >=
          codec_traits::packed_segment_size<Codec>::value) {
      write_segment(first, segment_unpacker);
      first += codec_traits::packed_segment_size<Codec>::value;
      bytes_append += codec_traits::unpacked_segment_size<Codec>::value;
    }

    fill_packed_segment(first, last, packed_segment_);
    return bytes_append;
  }

  template <typename Iterator, typename EndIterator, typename SegmentUnpacker>
  std::size_t write_segments(
      Iterator first,
      EndIterator last,
      SegmentUnpacker segment_unpacker,
      std::input_iterator_tag) {

    std::size_t bytes_append = 0;
    while(true) {
      if(!fill_packed_segment(first, last, packed_segment_))
        break;
      write_segment(packed_segment_.begin(), segment_unpacker);
      packed_segment_.clear();
      bytes_append += codec_traits::unpacked_segment_size<Codec>::value;
    }

    return bytes_append;
  }

  Codec const& codec_;
  OutputIterator out_;
  std::size_t bytes_written_;

  detail::
      segment_buffer<bits_type, codec_traits::packed_segment_size<Codec>::value>
          packed_segment_;
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

////
///-----------------------------------------------------------------------------
////
// template <
//    typename InputIterator,
//    typename InputEndIterator,
//    typename OutputIterator,
//    typename Codec>
// std::size_t encode2(
//    InputIterator first,
//    InputEndIterator last,
//    OutputIterator out,
//    Codec const& codec) {
//  using boost::radix::adl::get_segment_unpacker;
//  boost::radix::detail::encode_impl(
//      first, last,
//      ::boost::radix::detail::maybe_wrap_output_iterator(
//          codec, out,
//          typename codec_traits::requires_line_breaks<Codec>::type()),
//      codec, get_segment_unpacker(codec));
//}

}} // namespace boost::radix

#endif // BOOST_RADIX_ENCODE_HPP
