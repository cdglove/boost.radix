//
// boost/radix/decode.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_DECODE_HPP
#define BOOST_RADIX_DECODE_HPP

#include <boost/radix/common.hpp>

#include <boost/radix/codec_traits/pad.hpp>
#include <boost/radix/codec_traits/segment.hpp>
#include <boost/radix/codec_traits/validation.hpp>
#include <boost/radix/exception.hpp>
#include <boost/radix/static_obitstream_msb.hpp>

#include <boost/move/utility.hpp>
#include <cctype>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif

namespace boost { namespace radix {

namespace detail {

template <typename Codec>
bool handle_whitespace_character(
    Codec const& codec, char_type c, handler::assert) {
  BOOST_ASSERT(!std::isspace(c));
  return true;
}

template <typename Codec>
bool handle_whitespace_character(
    Codec const& codec, char_type c, handler::exception) {
  if(std::isspace(c)) {
    BOOST_THROW_EXCEPTION(invalid_whitespace());
  }
  return true;
}

template <typename Codec>
bool handle_whitespace_character(
    Codec const& codec, char_type c, handler::ignore) {
  if(std::isspace(c))
    return false;
  return true;
}

template <typename Codec>
bool handle_nonalphabet_character(
    Codec const& codec, char_type c, handler::assert) {
  BOOST_ASSERT(codec.has_char(c));
  return true;
}

template <typename Codec>
bool handle_nonalphabet_character(
    Codec const& codec, char_type c, handler::exception) {
  if(!codec.has_char(c)) {
    BOOST_THROW_EXCEPTION(nonalphabet_character());
  }
  return true;
}

template <typename Codec>
bool handle_nonalphabet_character(
    Codec const& codec, char_type c, handler::ignore) {
  if(!codec.has_char(c))
    return false;
  return true;
}

} // namespace detail

// -----------------------------------------------------------------------------
//
namespace adl {

template <typename Codec>
static_obitstream_msb<codec_traits::required_bits<Codec>::value>
get_segment_packer(Codec const& codec) {
  return static_obitstream_msb<codec_traits::required_bits<Codec>::value>();
}

template <typename Codec>
std::size_t get_decoded_size(std::size_t source_size, Codec const& codec) {
  using boost::radix::codec_traits::packed_segment_size;
  using boost::radix::codec_traits::unpacked_segment_size;

  // By default, size is an integer multiple of the output
  // segment size.
  return packed_segment_size<Codec>::value *
         (source_size + (unpacked_segment_size<Codec>::value - 1) /
                            unpacked_segment_size<Codec>::value);
}

template <typename Codec>
bool validate_nonalphabet_character(Codec const& codec, char_type c) {
  return ::boost::radix::detail::handle_nonalphabet_character(
      codec, c, typename codec_traits::on_whitespace_char<Codec>::type());
}

template <typename Codec>
bool validate_whitespace_character(Codec const& codec, char_type c) {
  return ::boost::radix::detail::handle_whitespace_character(
      codec, c, typename codec_traits::on_nonalphabet_char<Codec>::type());
}

template <typename Codec>
bool validate_character(Codec const& codec, char_type c) {
  return validate_whitespace_character(codec, c) &&
         validate_nonalphabet_character(codec, c);
}

} // namespace adl

namespace detail {

template <
    typename Codec,
    typename Iterator,
    typename EndIterator,
    typename UnpackedSegment>
bool fill_unpacked_segment(
    Codec const& codec,
    Iterator& first,
    EndIterator last,
    UnpackedSegment& unpacked) {
  using boost::radix::codec_traits::required_bits;

  typename UnpackedSegment::iterator ubegin = unpacked.end();
  typename UnpackedSegment::iterator uend   = ubegin + unpacked.capacity();

  while(first != last && ubegin != uend) {
    char_type c = *first++;

    using boost::radix::adl::validate_character;
    if(validate_character(codec, c)) {
      *ubegin++ = codec.bits_from_char(c);
    }
  }

  unpacked.resize(std::distance(unpacked.begin(), ubegin));
  std::fill(ubegin, uend, 0);
  return ubegin == uend && first != last;
}

template <
    typename Codec,
    typename UnpackedSegment,
    typename OutputIterator,
    typename SegmentPacker>
void pack_segment(
    Codec const&,
    UnpackedSegment const& unpacked_segment,
    OutputIterator& out,
    SegmentPacker packer) {
  boost::array<bits_type, codec_traits::packed_segment_size<Codec>::value>
      packed_segment;
  packer(unpacked_segment, packed_segment);
  out = std::copy(packed_segment.begin(), packed_segment.end(), out);
}

// -----------------------------------------------------------------------------
//
template <
    typename InputIterator,
    typename InputEndIterator,
    typename OutputIterator,
    typename Codec,
    typename SegmentPacker>
std::size_t decode_impl(
    InputIterator first,
    InputEndIterator last,
    OutputIterator out,
    Codec const& codec,
    SegmentPacker packer) {
  if(first == last)
    return 0;

  using boost::radix::codec_traits::packed_segment_size;
  using boost::radix::codec_traits::required_bits;
  using boost::radix::codec_traits::unpacked_segment_size;

  std::size_t bytes_written = 0;
  std::size_t ins           = 0;

  while(true) {
    detail::segment_buffer<bits_type, unpacked_segment_size<Codec>::value>
        unpacked_segment;
    ::boost::radix::detail::fill_unpacked_segment(
        codec, first, last, unpacked_segment);

    boost::array<bits_type, packed_segment_size<Codec>::value> packed_segment;
    packer(unpacked_segment, packed_segment);

    // If we hit the end, we can't write out everything.
    if(first == last) {
      typename detail::segment_buffer<
          bits_type, unpacked_segment_size<Codec>::value>::iterator pad =
          std::find(
              unpacked_segment.begin(), unpacked_segment.end(),
              codec.get_pad_bits());

      std::size_t output_size = 1;

      if(codec_traits::required_bits<Codec>::value > 1) {
        output_size = (std::distance(unpacked_segment.begin(), pad) *
                       codec_traits::required_bits<Codec>::value) /
                      8;
      }

      std::copy(
          packed_segment.begin(), packed_segment.begin() + output_size, out);

      bytes_written += output_size;
      break;
    }

    out = std::copy(packed_segment.begin(), packed_segment.end(), out);
    bytes_written += packed_segment.size();
  }

  return bytes_written;
}

} // namespace detail

template <typename Codec, typename OutputIterator>
class decoder {
 public:
  decoder(Codec const& codec, OutputIterator out)
      : codec_(codec)
      , out_(out)
      , bytes_written_(0) {
  }

  template <typename Iterator, typename EndIterator>
  std::size_t append(Iterator first, EndIterator last) {
    using boost::radix::adl::get_segment_packer;
    std::size_t bytes_appended =
        append_impl(first, last, get_segment_packer(codec_));
    bytes_written_ += bytes_appended;
    return bytes_appended;
  }

  std::size_t append(bits_type bits) {
    unpacked_segment_.push_back(bits);
    if(unpacked_segment_.full()) {
      using boost::radix::adl::get_segment_packer;
      ::boost::radix::detail::pack_segment(
          codec_, unpacked_segment_, out_, get_segment_packer(codec_));
      bytes_written_ += codec_traits::packed_segment_size<Codec>::value;
      unpacked_segment_.clear();
    }
    return 1;
  }

  std::size_t resolve() {
    if(unpacked_segment_.empty())
      return 0;

    typedef boost::array<
        char_type, codec_traits::packed_segment_size<Codec>::value>
        packed_segment_type;
    packed_segment_type packed_segment;
    typename packed_segment_type::iterator out_buf = packed_segment.begin();
    using boost::radix::adl::get_segment_packer;
    ::boost::radix::detail::pack_segment(
        codec_, unpacked_segment_, out_buf, get_segment_packer(codec_));

    typename unpacked_segment_type::iterator pad = std::find(
        unpacked_segment_.begin(), unpacked_segment_.end(),
        codec_.get_pad_bits());

    std::size_t output_size = 1;
    if(codec_traits::required_bits<Codec>::value > 1) {
      std::size_t unpacked_bytes =
          std::distance(unpacked_segment_.begin(), pad);
      std::size_t packed_bits =
          (unpacked_bytes * codec_traits::required_bits<Codec>::value);
      output_size = packed_bits / 8;
    }

    std::copy(
        packed_segment.begin(), packed_segment.begin() + output_size, out_);
    bytes_written_ += output_size;
    return output_size;
  }

  void abort() {
    unpacked_segment_.clear();
  }

  void reset(OutputIterator out) {
    abort();
    out_           = boost::move(out);
    bytes_written_ = 0;
  }

  std::size_t bytes_written() const {
    return bytes_written_;
  }

 private:
  //
  template <typename Iterator, typename EndIterator, typename SegmentPacker>
  std::size_t append_impl(
      Iterator first, EndIterator last, SegmentPacker segment_packer) {
    std::size_t bytes_appended = 0;
    if(!unpacked_segment_.empty()) {
      if(!::boost::radix::detail::fill_unpacked_segment(
             codec_, first, last, unpacked_segment_)) {
        return 0;
      }

      BOOST_ASSERT(unpacked_segment_.full());
      ::boost::radix::detail::pack_segment(
          codec_, unpacked_segment_, out_, segment_packer);
      unpacked_segment_.clear();
      bytes_appended += codec_traits::packed_segment_size<Codec>::value;
    }

    bytes_appended += direct_write_segments(
        first, last, segment_packer,
        typename std::iterator_traits<Iterator>::iterator_category());
    return bytes_appended;
  }

  // template <typename Iterator, typename SegmentPacker>
  // std::size_t direct_write_segments(
  //     Iterator first,
  //     Iterator last,
  //     SegmentPacker& segment_packer,
  //     std::random_access_iterator_tag) {
  //   std::size_t bytes_appended = 0;
  //   while(std::distance(first, last) >=
  //         codec_traits::unpacked_segment_size<Codec>::value) {
  //     bytes_appended += ::boost::radix::detail::pack_segment(
  //         codec_, first, out_, segment_packer);
  //     bytes_appended += codec_traits::packed_segment_size<Codec>::value;
  //   }

  //   fill_packed_segment(first, last, packed_segment_);
  //   return bytes_appended;
  // }

  template <typename Iterator, typename EndIterator, typename SegmentPacker>
  std::size_t direct_write_segments(
      Iterator first, EndIterator last, SegmentPacker segment_packer, ...) {
    std::size_t bytes_appended = 0;
    while(true) {
      if(!::boost::radix::detail::fill_unpacked_segment(
             codec_, first, last, unpacked_segment_))
        break;
      ::boost::radix::detail::pack_segment(
          codec_, unpacked_segment_, out_, segment_packer);
      unpacked_segment_.clear();
      bytes_appended += codec_traits::packed_segment_size<Codec>::value;
    }

    return bytes_appended;
  }

  Codec const& codec_;
  OutputIterator out_;
  std::size_t bytes_written_;

  typedef detail::segment_buffer<
      bits_type,
      codec_traits::unpacked_segment_size<Codec>::value>
      unpacked_segment_type;
  unpacked_segment_type unpacked_segment_;
}; // namespace radix

template <typename Codec, typename OutputIterator>
decoder<Codec, OutputIterator> make_decoder(
    Codec const& codec, OutputIterator out) {
  return decoder<Codec, OutputIterator>(codec, out);
}

// -----------------------------------------------------------------------------
//
template <typename Codec>
std::size_t decoded_size(std::size_t source_size, Codec const& codec) {
  using boost::radix::adl::get_decoded_size;
  return get_decoded_size(source_size, codec);
}

// -----------------------------------------------------------------------------
//
template <
    typename InputIterator,
    typename InputEndIterator,
    typename OutputIterator,
    typename Codec>
std::size_t decode(
    InputIterator first,
    InputEndIterator last,
    OutputIterator out,
    Codec const& codec) {
  // using boost::radix::adl::get_segment_packer;
  // return boost::radix::detail::decode_impl(
  //     first, last, out, codec, get_segment_packer(codec));
  decoder<Codec, OutputIterator> d(codec, out);
  d.append(first, last);
  d.resolve();
  return d.bytes_written();
}

}} // namespace boost::radix

#endif // BOOST_RADIX_DECODE_HPP
