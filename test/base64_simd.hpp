#include <boost/beast/core/detail/base64.hpp>
#include <cstdint>
#include <emmintrin.h>
#include <tmmintrin.h>

namespace boost { namespace beast { namespace detail { namespace base64 {

inline __m128i or_all(__m128i a, __m128i b, __m128i c) {
  return _mm_or_si128(_mm_or_si128(a, b), c);
}

inline __m128i or_all(__m128i a, __m128i b, __m128i c, __m128i d) {
  return _mm_or_si128(_mm_or_si128(_mm_or_si128(a, b), c), d);
}

inline std::size_t encode_simd(void* dest, void const* src, std::size_t len) {
  char* out      = static_cast<char*>(dest);
  char const* in = static_cast<char const*>(src);
  auto const tab = get_alphabet();

  boost::array<__m128i, 3> src_simd;
  boost::array<__m128i, 3> packed_simd;
  boost::array<__m128i, 4> unpacked_simd;
  boost::array<__m128i, 4> dest_simd;

  __m128i const mask_0x3f = _mm_set1_epi8(0x3f);
  __m128i const mask_0x30 = _mm_set1_epi8(0x30);
  __m128i const mask_0x0f = _mm_set1_epi8(0x0f);
  __m128i const mask_0x03 = _mm_set1_epi8(0x03);
  __m128i const mask_0x3c = _mm_set1_epi8(0x3c);

  // clang-format off
  boost::array<boost::array<__m128i,3>, 3> const shuffle_packed = {
    _mm_setr_epi8(0, 3, 6,  9, 12, 15,   0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80),
    _mm_setr_epi8(1, 4, 7, 10, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80),
    _mm_setr_epi8(2, 5, 8, 11, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80),

    _mm_setr_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 2, 5, 8,  11, 14,   0x80, 0x80, 0x80, 0x80, 0x80),
    _mm_setr_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0,    3, 6, 9,  12, 15,   0x80, 0x80, 0x80, 0x80, 0x80),
    _mm_setr_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 1,    4, 7, 10, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80),

    _mm_setr_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 1, 4, 7, 10, 13),
    _mm_setr_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 2, 5, 8, 11, 14),
    _mm_setr_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0,    3, 6, 9, 12, 15),
  };

  boost::array<boost::array<__m128i,4>, 4> const shuffle_unpacked = {
    _mm_setr_epi8(0, 0x80, 0x80, 0x80, 1, 0x80, 0x80, 0x80, 2, 0x80, 0x80, 0x80, 3, 0x80, 0x80, 0x80),
    _mm_setr_epi8(0x80, 0, 0x80, 0x80, 0x80, 1, 0x80, 0x80, 0x80, 2, 0x80, 0x80, 0x80, 3, 0x80, 0x80),
    _mm_setr_epi8(0x80, 0x80, 0, 0x80, 0x80, 0x80, 1, 0x80, 0x80, 0x80, 2, 0x80, 0x80, 0x80, 3, 0x80),
    _mm_setr_epi8(0x80, 0x80, 0x80, 0, 0x80, 0x80, 0x80, 1, 0x80, 0x80, 0x80, 2, 0x80, 0x80, 0x80, 3),

    _mm_setr_epi8(4, 0x80, 0x80, 0x80, 5, 0x80, 0x80, 0x80, 6, 0x80, 0x80, 0x80, 7, 0x80, 0x80, 0x80),
    _mm_setr_epi8(0x80, 4, 0x80, 0x80, 0x80, 5, 0x80, 0x80, 0x80, 6, 0x80, 0x80, 0x80, 7, 0x80, 0x80),
    _mm_setr_epi8(0x80, 0x80, 4, 0x80, 0x80, 0x80, 5, 0x80, 0x80, 0x80, 6, 0x80, 0x80, 0x80, 7, 0x80),
    _mm_setr_epi8(0x80, 0x80, 0x80, 4, 0x80, 0x80, 0x80, 5, 0x80, 0x80, 0x80, 6, 0x80, 0x80, 0x80, 7),

    _mm_setr_epi8(8, 0x80, 0x80, 0x80, 9, 0x80, 0x80, 0x80, 10, 0x80, 0x80, 0x80, 11, 0x80, 0x80, 0x80),
    _mm_setr_epi8(0x80, 8, 0x80, 0x80, 0x80, 9, 0x80, 0x80, 0x80, 10, 0x80, 0x80, 0x80, 11, 0x80, 0x80),
    _mm_setr_epi8(0x80, 0x80, 8, 0x80, 0x80, 0x80, 9, 0x80, 0x80, 0x80, 10, 0x80, 0x80, 0x80, 11, 0x80),
    _mm_setr_epi8(0x80, 0x80, 0x80, 8, 0x80, 0x80, 0x80, 9, 0x80, 0x80, 0x80, 10, 0x80, 0x80, 0x80, 11),

    _mm_setr_epi8(12, 0x80, 0x80, 0x80, 13, 0x80, 0x80, 0x80, 14, 0x80, 0x80, 0x80, 15, 0x80, 0x80, 0x80),
    _mm_setr_epi8(0x80, 12, 0x80, 0x80, 0x80, 13, 0x80, 0x80, 0x80, 14, 0x80, 0x80, 0x80, 15, 0x80, 0x80),
    _mm_setr_epi8(0x80, 0x80, 12, 0x80, 0x80, 0x80, 13, 0x80, 0x80, 0x80, 14, 0x80, 0x80, 0x80, 15, 0x80),
    _mm_setr_epi8(0x80, 0x80, 0x80, 12, 0x80, 0x80, 0x80, 13, 0x80, 0x80, 0x80, 14, 0x80, 0x80, 0x80, 15),
  };
  // clang-format on

  for(auto n = len / (3 * 16); n--;) {

    src_simd[0] = _mm_loadu_si128(reinterpret_cast<__m128i const*>(in));
    in += 16;
    src_simd[1] = _mm_loadu_si128(reinterpret_cast<__m128i const*>(in));
    in += 16;
    src_simd[2] = _mm_loadu_si128(reinterpret_cast<__m128i const*>(in));
    in += 16;

    packed_simd[0] = _mm_or_si128(
        _mm_or_si128(
            _mm_shuffle_epi8(src_simd[0], shuffle_packed[0][0]),
            _mm_shuffle_epi8(src_simd[1], shuffle_packed[0][1])),
        _mm_shuffle_epi8(src_simd[2], shuffle_packed[0][2]));

    packed_simd[1] = _mm_or_si128(
        _mm_or_si128(
            _mm_shuffle_epi8(src_simd[0], shuffle_packed[1][0]),
            _mm_shuffle_epi8(src_simd[1], shuffle_packed[1][1])),
        _mm_shuffle_epi8(src_simd[2], shuffle_packed[1][2]));

    packed_simd[2] = _mm_or_si128(
        _mm_or_si128(
            _mm_shuffle_epi8(src_simd[0], shuffle_packed[2][0]),
            _mm_shuffle_epi8(src_simd[1], shuffle_packed[2][1])),
        _mm_shuffle_epi8(src_simd[2], shuffle_packed[2][2]));

    unpacked_simd[0] =
        _mm_and_si128(_mm_srli_epi16(packed_simd[0], 2), mask_0x3f);
    unpacked_simd[1] = _mm_or_si128(
        _mm_and_si128(_mm_slli_epi16(packed_simd[0], 4), mask_0x30),
        _mm_and_si128(_mm_srli_epi16(packed_simd[1], 4), mask_0x0f));
    unpacked_simd[2] = _mm_or_si128(
        _mm_and_si128(_mm_slli_epi16(packed_simd[1], 2), mask_0x3c),
        _mm_and_si128(_mm_srli_epi16(packed_simd[2], 6), mask_0x03));
    unpacked_simd[3] = _mm_and_si128(packed_simd[2], mask_0x3f);

    dest_simd[0] = _mm_or_si128(
        _mm_or_si128(
            _mm_or_si128(
                _mm_shuffle_epi8(unpacked_simd[0], shuffle_unpacked[0][0]),
                _mm_shuffle_epi8(unpacked_simd[1], shuffle_unpacked[1][0])),
            _mm_shuffle_epi8(unpacked_simd[2], shuffle_unpacked[2][0])),
        _mm_shuffle_epi8(unpacked_simd[3], shuffle_unpacked[3][0]));

    dest_simd[1] = _mm_or_si128(
        _mm_or_si128(
            _mm_or_si128(
                _mm_shuffle_epi8(unpacked_simd[0], shuffle_unpacked[0][1]),
                _mm_shuffle_epi8(unpacked_simd[1], shuffle_unpacked[1][1])),
            _mm_shuffle_epi8(unpacked_simd[2], shuffle_unpacked[2][1])),
        _mm_shuffle_epi8(unpacked_simd[3], shuffle_unpacked[3][1]));

    dest_simd[2] = _mm_or_si128(
        _mm_or_si128(
            _mm_or_si128(
                _mm_shuffle_epi8(unpacked_simd[0], shuffle_unpacked[0][2]),
                _mm_shuffle_epi8(unpacked_simd[1], shuffle_unpacked[1][2])),
            _mm_shuffle_epi8(unpacked_simd[2], shuffle_unpacked[2][2])),
        _mm_shuffle_epi8(unpacked_simd[3], shuffle_unpacked[3][2]));

    dest_simd[3] = _mm_or_si128(
        _mm_or_si128(
            _mm_or_si128(
                _mm_shuffle_epi8(unpacked_simd[0], shuffle_unpacked[0][3]),
                _mm_shuffle_epi8(unpacked_simd[1], shuffle_unpacked[1][3])),
            _mm_shuffle_epi8(unpacked_simd[2], shuffle_unpacked[2][3])),
        _mm_shuffle_epi8(unpacked_simd[3], shuffle_unpacked[3][3]));

    boost::array<unsigned char, 16 * 4> pre_map_buf;
    _mm_storeu_si128(
        reinterpret_cast<__m128i*>(pre_map_buf.data()), dest_simd[0]);
    _mm_storeu_si128(
        reinterpret_cast<__m128i*>(pre_map_buf.data() + 16), dest_simd[1]);
    _mm_storeu_si128(
        reinterpret_cast<__m128i*>(pre_map_buf.data() + 32), dest_simd[2]);
    _mm_storeu_si128(
        reinterpret_cast<__m128i*>(pre_map_buf.data() + 48), dest_simd[3]);

    // Copying to a secondary buffer is faster than outputting
    // directly.
    boost::array<unsigned char, 16 * 4> mapped_buf;
    for(std::size_t i = 0; i < pre_map_buf.size(); ++i) {
      mapped_buf[i] = pre_map_buf[tab[i]];
    }

    for(std::size_t i = 0; i < mapped_buf.size(); ++i) {
      *out++ = mapped_buf[i];
    }
  }

  len = len % (3 * 16);

  // The rest.
  for(auto n = len / 3; n--;) {
    *out++ = tab[(in[0] & 0xfc) >> 2];
    *out++ = tab[((in[0] & 0x03) << 4) + ((in[1] & 0xf0) >> 4)];
    *out++ = tab[((in[2] & 0xc0) >> 6) + ((in[1] & 0x0f) << 2)];
    *out++ = tab[in[2] & 0x3f];
    in += 3;
  }

  switch(len % 3) {
  case 2:
    *out++ = tab[(in[0] & 0xfc) >> 2];
    *out++ = tab[((in[0] & 0x03) << 4) + ((in[1] & 0xf0) >> 4)];
    *out++ = tab[(in[1] & 0x0f) << 2];
    *out++ = '=';
    break;

  case 1:
    *out++ = tab[(in[0] & 0xfc) >> 2];
    *out++ = tab[((in[0] & 0x03) << 4)];
    *out++ = '=';
    *out++ = '=';
    break;

  case 0:
    break;
  }

  return out - static_cast<char*>(dest);
}

}}}} // namespace boost::beast::detail::base64