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
      unpack_segment(packed_segment_.begin(), get_segment_unpacker(codec_));
      bytes_written_ += UnpackedSegmentSize;
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

    boost::array<char_type, UnpackedSegmentSize> unpacked_segment;
    using boost::radix::adl::get_segment_unpacker;
    get_segment_unpacker(codec_)(packed_segment_.begin(), unpacked_segment);

    std::size_t unpacked_size =
        maybe_pad_segment(packed_segment_.size(), unpacked_segment);
    packed_segment_.clear();

    out_ = std::transform(
        unpacked_segment.begin(), unpacked_segment.begin() + unpacked_size,
        out_, bits_to_char_mapper(codec_));

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
      unpack_segment(packed_segment_.begin(), segment_unpacker);
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
    std::size_t full_segment_count =
        std::distance(first, last) / PackedSegmentSize;
    bytes_appended += full_segment_count * UnpackedSegmentSize;

    while(full_segment_count--) {
      unpack_segment(first, segment_unpacker);
      first += PackedSegmentSize;
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
      BOOST_ASSERT(packed_segment_.full());
      unpack_segment(packed_segment_.begin(), segment_unpacker);
      packed_segment_.clear();
      bytes_appended += UnpackedSegmentSize;
    }

    return bytes_appended;
  }

  template <typename Iterator, typename EndIterator, typename PackedSegment>
  bool fill_packed_segment(
      Iterator& first, EndIterator last, PackedSegment& packed) {
    typename PackedSegment::iterator pbegin = packed.end();
    typename PackedSegment::iterator pend = packed.begin() + packed.capacity();

    while(first != last && pbegin != pend) {
      *pbegin++ = *first++;
    }

    packed.resize(std::distance(packed.begin(), pbegin));
    return pbegin == pend;
  }

  template <typename RandomAccessInputIterator, typename SegmentUnpacker>
  void unpack_segment(
      RandomAccessInputIterator from, SegmentUnpacker segment_unpacker) {
    typedef boost::array<
        bits_type, codec_traits::unpacked_segment_size<Codec>::value>
        buffer_type;
    buffer_type buffer;
    segment_unpacker(from, buffer);
    format_segment(buffer.begin(), buffer.end());
  }

  struct bits_to_char_mapper {
    bits_to_char_mapper(Codec const& codec)
        : codec_(codec) {
    }

    char_type operator()(bits_type bits) const {
      return codec_.char_from_bits(bits);
    }

    Codec const& codec_;
  };

  template <typename InputIterator>
  void format_segment(InputIterator first, InputIterator last) {
    out_ = std::transform(
        first, last,
        maybe_add_line_break_iterator(
            typename codec_traits::requires_line_breaks<Codec>::type()),
        bits_to_char_mapper(codec_));
  }

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

  line_break_iterator<
      OutputIterator,
      codec_traits::max_encoded_line_length<Codec>::value>
  maybe_add_line_break_iterator(boost::true_type) {
    return line_break_iterator<
        OutputIterator, codec_traits::max_encoded_line_length<Codec>::value>(
        out_);
  }

  OutputIterator maybe_add_line_break_iterator(boost::false_type) {
    return out_;
  }

  std::size_t get_unpacked_size_from_packed_size(std::size_t packed_size) {
    std::size_t bits_written = packed_size * 8;
    std::size_t bytes_written =
        (bits_written + (codec_traits::required_bits<Codec>::value - 1)) /
        codec_traits::required_bits<Codec>::value;
    return bytes_written;
  }

  static const std::size_t PackedSegmentSize =
      codec_traits::packed_segment_size<Codec>::value;
  static const std::size_t UnpackedSegmentSize =
      codec_traits::unpacked_segment_size<Codec>::value;

  std::size_t maybe_pad_segment_impl(
      std::size_t packed_size,
      boost::array<char_type, UnpackedSegmentSize>& unpacked,
      boost::true_type) {
    std::size_t bytes_written = get_unpacked_size_from_packed_size(packed_size);

    while(bytes_written < unpacked.size())
      unpacked[bytes_written++] = codec_.get_pad_bits();

    return unpacked.size();
  }

  std::size_t maybe_pad_segment_impl(
      std::size_t packed_size,
      boost::array<char_type, UnpackedSegmentSize>& unpacked,
      boost::false_type) {
    return get_unpacked_size_from_packed_size(packed_size);
  }

  std::size_t maybe_pad_segment(
      std::size_t packed_size,
      boost::array<char_type, UnpackedSegmentSize>& unpacked) {
    return maybe_pad_segment_impl(
        packed_size, unpacked,
        typename codec_traits::requires_pad<Codec>::type());
  }

  Codec const& codec_;
  OutputIterator out_;
  std::size_t bytes_written_;

  typedef detail::segment_buffer<bits_type, PackedSegmentSize>
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
