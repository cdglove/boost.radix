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
#include <boost/radix/exception.hpp>
#include <boost/radix/static_obitstream_msb.hpp>

#if BOOST_RADIX_SUPPORT_BOOSTERRORCODE
#  include <boost/system/error_code.hpp>
#endif

#include <boost/move/utility.hpp>
#include <cctype>

#if BOOST_RADIX_SUPPORT_STDERRORCODE
#  include <system_error>
#endif

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif

namespace boost { namespace radix { namespace decode_validation {

enum op {
  op_consume,
  op_skip,
  op_abort,
};

enum error {
  none = 0,
  invalid_whitespace,
  nonalphabet_character,
};

}}} // namespace boost::radix::decode_validation

#if BOOST_RADIX_SUPPORT_STDERRORCODE
namespace std {
template <>
struct is_error_code_enum<boost::radix::decode_validation::error>
    : true_type {};
} // namespace std
#endif // BOOST_RADIX_SUPPORT_STDERRORCODE

#if BOOST_RADIX_SUPPORT_BOOSTERRORCODE
namespace boost { namespace system {
template <>
struct is_error_condition_enum<boost::radix::decode_validation::error> {
  static const bool value = true;
};
}}     // namespace boost::system
#endif // BOOST_RADIX_SUPPORT_BOOSTERRORCODE

namespace boost { namespace radix {

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
bool is_invalid_whitespace_character(Codec const& codec, char_type c) {
  if(std::isspace(c)) {
    return true;
  }
  return false;
}

template <typename Codec, typename ErrorHandler>
decode_validation::op validate_character(
    Codec const& codec, char_type c, ErrorHandler& errh) {
  if(is_invalid_whitespace_character(codec, c))
    return errh.handle_whitespace_character(codec, c);
  if(!codec.has_char(c))
    return errh.handle_nonalphabet_character(codec, c);
  return decode_validation::op_consume;
}

} // namespace adl

struct decode_error_handler_throw {
  template <typename Codec>
  decode_error_handler_throw(Codec const&) {
  }

  template <typename Codec>
  decode_validation::op handle_whitespace_character(Codec const&, char_type c) {
    BOOST_THROW_EXCEPTION(invalid_whitespace(c));
    return decode_validation::op_consume;
  }

  template <typename Codec>
  decode_validation::op handle_nonalphabet_character(
      Codec const& codec, char_type c) {
    BOOST_THROW_EXCEPTION(nonalphabet_character(c));
    return decode_validation::op_consume;
  }
};

template <typename ErrorCodeType>
struct decode_error_handler_error_code {
  template <typename Codec>
  decode_error_handler_error_code(Codec const&, ErrorCodeType& errc)
      : errc_(errc) {
  }

  template <typename Codec>
  decode_validation::op handle_whitespace_character(Codec const&, char_type c) {
    errc_ = decode_validation::invalid_whitespace;
    return decode_validation::op_abort;
  }

  template <typename Codec>
  decode_validation::op handle_nonalphabet_character(
      Codec const& codec, char_type c) {
    errc_ = decode_validation::nonalphabet_character;
    return decode_validation::op_abort;
  }

  ErrorCodeType& errc_;
};

struct decode_error_handler_assert {
  template <typename Codec>
  decode_error_handler_assert(Codec const&) {
  }

  template <typename Codec>
  decode_validation::op handle_whitespace_character(Codec const&, char_type c) {
    BOOST_ASSERT(false);
    return decode_validation::op_consume;
  }

  template <typename Codec>
  decode_validation::op handle_nonalphabet_character(
      Codec const& codec, char_type c) {
    BOOST_ASSERT(false);
    return decode_validation::op_consume;
  }
};

struct decode_error_handler_ignore {
  template <typename Codec>
  decode_error_handler_ignore(Codec const&) {
  }

  template <typename Codec>
  decode_validation::op handle_whitespace_character(Codec const&, char_type c) {
    return decode_validation::op_consume;
  }

  template <typename Codec>
  decode_validation::op handle_nonalphabet_character(
      Codec const& codec, char_type c) {
    return decode_validation::op_consume;
  }
};

// -----------------------------------------------------------------------------
//
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
    decode_error_handler_throw errh(codec_);
    std::size_t bytes_appended =
        append_impl(first, last, get_segment_packer(codec_), errh);
    bytes_written_ += bytes_appended;
    return bytes_appended;
  }

  std::size_t append(char_type c) {
    return append(&c, (&c) + 1);
  }

#if BOOST_RADIX_SUPPORT_BOOSTERRORCODE
  template <typename Iterator, typename EndIterator>
  std::size_t append(
      Iterator first, EndIterator last, boost::system::error_code& errc) {
    using boost::radix::adl::get_segment_packer;
    decode_error_handler_error_code<boost::system::error_code> errh(
        codec_, errc);
    std::size_t bytes_appended =
        append_impl(first, last, get_segment_packer(codec_), errh);
    bytes_written_ += bytes_appended;
    return bytes_appended;
  }

  std::size_t append(char_type c, boost::system::error_code& errc) {
    return append(&c, (&c) + 1, errc);
  }
#endif

#if BOOST_RADIX_SUPPORT_STDERRORCODE
  template <typename Iterator, typename EndIterator>
  std::size_t append(Iterator first, EndIterator last, std::error_code& errc) {
    using boost::radix::adl::get_segment_packer;
    decode_error_handler_error_code<std::error_code> errh(codec_, errc);
    std::size_t bytes_appended =
        append_impl(first, last, get_segment_packer(codec_), errh);
    bytes_written_ += bytes_appended;
    return bytes_appended;
  }

  std::size_t append(char_type c, std::error_code& errc) {
    return append(&c, (&c) + 1, errc);
  }
#endif

  std::size_t resolve() {
    if(unpacked_segment_.empty())
      return 0;

    typedef boost::array<
        char_type, codec_traits::packed_segment_size<Codec>::value>
        packed_segment_type;
    packed_segment_type packed_segment;
    using boost::radix::adl::get_segment_packer;
    get_segment_packer(codec_)(
        unpacked_segment_.begin(), packed_segment.begin());

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

    out_ = std::copy(
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
  template <
      typename Iterator,
      typename EndIterator,
      typename SegmentPacker,
      typename ErrorHandler>
  std::size_t append_impl(
      Iterator first,
      EndIterator last,
      SegmentPacker segment_packer,
      ErrorHandler& errh) {
    std::size_t bytes_appended = 0;
    if(!unpacked_segment_.empty()) {
      if(!fill_unpacked_segment(first, last, errh)) {
        return 0;
      }

      BOOST_ASSERT(unpacked_segment_.full());
      out_ = segment_packer(unpacked_segment_.begin(), out_);
      unpacked_segment_.clear();
      bytes_appended += codec_traits::packed_segment_size<Codec>::value;
    }

    bytes_appended += direct_write_segments(first, last, segment_packer, errh);
    return bytes_appended;
  }

  template <
      typename Iterator,
      typename EndIterator,
      typename SegmentPacker,
      typename ErrorHandler>
  std::size_t direct_write_segments(
      Iterator first,
      EndIterator last,
      SegmentPacker segment_packer,
      ErrorHandler& errh) {
    std::size_t bytes_appended = 0;
    while(true) {
      if(!fill_unpacked_segment(first, last, errh))
        break;
      BOOST_ASSERT(unpacked_segment_.full());
      out_ = segment_packer(unpacked_segment_.begin(), out_);
      ;
      unpacked_segment_.clear();
      bytes_appended += codec_traits::packed_segment_size<Codec>::value;
    }

    return bytes_appended;
  }

  template <typename Iterator, typename EndIterator, typename ErrorHandler>
  bool fill_unpacked_segment(
      Iterator& first, EndIterator last, ErrorHandler& errh) {
    using boost::radix::codec_traits::required_bits;

    typename unpacked_segment_type::iterator ubegin = unpacked_segment_.end();
    typename unpacked_segment_type::iterator uend =
        ubegin + unpacked_segment_.capacity();

    while(first != last && ubegin != uend) {
      char_type c = *first++;
      using boost::radix::adl::validate_character;
      switch(validate_character(codec_, c, errh)) {
      case decode_validation::op_consume:
        *ubegin++ = codec_.bits_from_char(c);
        break;
      case decode_validation::op_skip:
        continue;
      case decode_validation::op_abort:
        return false;
      }
    }

    unpacked_segment_.resize(std::distance(unpacked_segment_.begin(), ubegin));
    std::fill(ubegin, uend, 0);
    return ubegin == uend && first != last;
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
  decoder<Codec, OutputIterator> d(codec, out);
  d.append(first, last);
  d.resolve();
  return d.bytes_written();
}

#if BOOST_RADIX_SUPPORT_BOOSTERRORCODE
template <
    typename InputIterator,
    typename InputEndIterator,
    typename OutputIterator,
    typename Codec>
std::size_t decode(
    InputIterator first,
    InputEndIterator last,
    OutputIterator out,
    Codec const& codec,
    boost::system::error_code& errc) {
  decoder<Codec, OutputIterator> d(codec, out);
  d.append(first, last, errc);
  d.resolve();
  return d.bytes_written();
}
#endif

#if BOOST_RADIX_SUPPORT_STDERRORCODE
template <
    typename InputIterator,
    typename InputEndIterator,
    typename OutputIterator,
    typename Codec>
std::size_t decode(
    InputIterator first,
    InputEndIterator last,
    OutputIterator out,
    Codec const& codec,
    std::error_code& errc) {
  decoder<Codec, OutputIterator> d(codec, out);
  d.append(first, last, errc);
  d.resolve();
  return d.bytes_written();
}
#endif

}} // namespace boost::radix

#endif // BOOST_RADIX_DECODE_HPP
