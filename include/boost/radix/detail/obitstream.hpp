//
// boost/radix/detail/obitstream.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_DETAIL_OBITSTREAM_HPP
#define BOOST_RADIX_DETAIL_OBITSTREAM_HPP

#include <boost/radix/common.hpp>

#include <algorithm>
#include <boost/align/align_up.hpp>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/make_unsigned.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#    pragma once
#endif

namespace boost { namespace radix { namespace detail {

/// \class ibitstream
template <typename OutputIterator>
class obitstream
{
public:
    obitstream(OutputIterator out)
        : current_(out)
        , bit_(0)
    {
        *current_ = 0;
    }

    template <typename ValueType, typename BitWriter>
    void write_bits(ValueType const& v, BitWriter writer)
    {
        BOOST_ASSERT(writer.num_bits() <= sizeof(v) * 8);

        typedef typename std::iterator_traits<OutputIterator>::value_type
            buffer_element_type;

        // For now, we only support bytes.
        BOOST_STATIC_ASSERT(sizeof(buffer_element_type) == 1);

        std::size_t const element_bits = sizeof(buffer_element_type) * 8;

        buffer_element_type all_element_bit_mask = 0;
        all_element_bit_mask                     = ~all_element_bit_mask;

        if(bit_ == 0)
        {
            *current_ = 0;
        }

        *current_ |= (v & all_element_bit_mask) << bit_;

        std::size_t bits_inserted = element_bits - bit_;

        if(bits_inserted < writer.num_bits())
        {
            typedef typename boost::make_unsigned<ValueType>::type
                unsigned_value_type;
            unsigned_value_type unsigned_v(v);
            unsigned_v >>= bits_inserted;

            std::size_t bits_remaining = writer.num_bits() - bits_inserted;
            std::size_t aligned_bits_remaining =
                boost::alignment::align_up(bits_remaining, element_bits);
            std::size_t elements_remaining =
                aligned_bits_remaining / element_bits;
            BOOST_ASSERT(elements_remaining <= sizeof(v));

            // cglover-note: There's opportunity for optimization here if we
            // know the endiness of the machine.
            while(elements_remaining--)
            {
                ++current_;
                *current_ = unsigned_v;
                unsigned_v >>= 8;
            }
        }

        bit_ += writer.num_bits();
        while(bit_ >= 8)
        {
            bit_ -= 8;
        }

        if(bit_ == 0)
        {
            ++current_;
        }
    }

private:
    BOOST_STATIC_ASSERT(
        boost::is_integral<
            typename std::iterator_traits<OutputIterator>::value_type>::value);

    OutputIterator current_;
    std::size_t bit_;
};

class dynamic_bit_writer
{
public:
    dynamic_bit_writer(std::size_t num_bits)
        : num_bits_(num_bits)
    {}

    template <typename SourceType>
    typename boost::make_unsigned<SourceType>::type
    operator()(SourceType const& source, std::size_t source_bit_offset) const
    {
        return source << source_bit_offset;
    }

    template <typename SourceType>
    typename boost::make_unsigned<SourceType>::type
    operator()(SourceType const& source) const
    {
        return source;
    }

    std::size_t num_bits() const
    {
        return num_bits_;
    }

private:
    std::size_t num_bits_;
};

}}} // namespace boost::radix::detail

#endif // BOOST_RADIX_DETAIL_OBITSTREAM_HPP
