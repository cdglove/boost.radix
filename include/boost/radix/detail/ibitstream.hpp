//
// boost/radix/detail/ibitstream.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_DETAIL_IBITSTREAM_HPP
#define BOOST_RADIX_DETAIL_IBITSTREAM_HPP

#include <boost/config.hpp>

#include <algorithm>
#include <boost/align/align_up.hpp>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/make_unsigned.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost { namespace radix { namespace detail {

/// \class ibitstream
template <typename InputIterator>
class ibitstream
{
public:
    ibitstream(InputIterator first, InputIterator last)
        : current_(first)
        , end_(last)
        , bit_(0)
    {}

    template <typename ValueType, typename BitReader>
    void read_bits(ValueType& v, BitReader reader)
    {
        BOOST_ASSERT(reader.num_bits() <= sizeof(v) * 8);
        BOOST_ASSERT(reader.num_bits() > 0);

        typedef typename std::iterator_traits<InputIterator>::value_type
            buffer_element_type;
        typedef typename boost::make_unsigned<buffer_element_type>::type
            unsigned_buffer_element_type;

        // For now, we only support bytes.
        BOOST_STATIC_ASSERT(sizeof(buffer_element_type) == 1);

        std::size_t const element_bits = sizeof(buffer_element_type) * 8;

        // Extract whatever is left in the current element.
        v = reader(*current_, bit_);

        std::size_t bits_extracted = element_bits - bit_;

        if(bits_extracted == reader.num_bits())
        {
            ++current_;
            bit_ = 0;
            return;
        }
        else if(bits_extracted < reader.num_bits())
        {
            std::size_t bits_remaining = reader.num_bits() - bits_extracted;
            std::size_t aligned_bits_remaining =
                boost::alignment::align_up(bits_remaining, element_bits);
            std::size_t elements_remaining =
                aligned_bits_remaining / element_bits;
            BOOST_ASSERT(elements_remaining <= sizeof(v));

            // cglover-note: There's opportunity for optimization here if we
            // know the endiness of the machine.
            while(elements_remaining-- && (++current_, true) && !finished())
            {
                ValueType rest = reader(*current_);
                v |= rest << bits_extracted;
                bits_extracted += element_bits;
            }
        }

        ValueType all_value_bit_mask = 0;
        all_value_bit_mask           = ~all_value_bit_mask;

        // Mask off any overhang we didn't ask for.
        v &= ~(all_value_bit_mask << reader.num_bits());

        // Reset the bit offset to account for overhang.
        bit_ += reader.num_bits();
        while(bit_ >= 8)
        {
            bit_ -= 8;
        }

        if(!finished() && bit_ == 0)
        {
            ++current_;
        }
    }

    bool finished() const
    {
        return current_ == end_;
    }

private:
    BOOST_STATIC_ASSERT(
        boost::is_integral<
            typename std::iterator_traits<InputIterator>::value_type>::value);

    InputIterator current_;
    InputIterator end_;
    std::size_t bit_;
};

class ibitstream_le
{
public:
    template <typename Iterator>
    ibitstream_le(Iterator& current)
        : byte_(*current++)
        , bit_(0)
    {}

    template <typename ValueType, typename Iterator, typename BitReader>
    void
    read_bits(ValueType& v, Iterator& next, Iterator last, BitReader reader)
    {
        BOOST_ASSERT(reader.num_bits() <= sizeof(v) * 8);
        BOOST_ASSERT(reader.num_bits() > 0);

        std::size_t const element_bits = 8;

        v = reader(byte_, bit_);

        std::size_t bits_extracted = element_bits - bit_;

        //// Extract whatever is left in the current element.
        // v = reader(byte_, bit_);
        //
        // std::size_t bits_extracted = element_bits - bit_;

        // if(bits_extracted == reader.num_bits())
        //{
        //    byte_ = *current_++;
        //    bit_ = 0;
        //    return;
        //}
        // else if(bits_extracted < reader.num_bits())
        //{
        //    std::size_t bits_remaining = reader.num_bits() - bits_extracted;
        //    std::size_t aligned_bits_remaining =
        //    boost::alignment::align_up(bits_remaining, element_bits);
        //    std::size_t elements_remaining = aligned_bits_remaining /
        //    element_bits; BOOST_ASSERT(elements_remaining <= sizeof(v));
        //
        //    // cglover-note: There's opportunity for optimization here if we
        //    know
        //    // the endiness of the machine.
        //    while(elements_remaining-- && (++current_, true) && (current !=
        //    last))
        //    {
        //        ValueType rest = reader(*current_);
        //        v |= rest << bits_extracted;
        //        bits_extracted += element_bits;
        //    }
        //}

        // ValueType all_value_bit_mask = 0;
        // all_value_bit_mask = ~all_value_bit_mask;

        //// Mask off any overhang we didn't ask for.
        // v &= ~(all_value_bit_mask << reader.num_bits());

        //// Reset the bit offset to account for overhang.
        // bit_ += reader.num_bits();
        // while(bit_ >= 8)
        //{
        //    bit_ -= 8;
        //}

        // if((current != last) && bit_ == 0)
        //{
        //    byte_ = *current++;
        //
        //}
    }

    // bool finished() const
    //{
    //    return current_ == end_;
    //}

private:
    bits_type byte_;
    std::size_t bit_;
};

class dynamic_bit_reader
{
public:
    dynamic_bit_reader(std::size_t num_bits)
        : num_bits_(num_bits)
    {}

    template <typename SourceType>
    typename boost::make_unsigned<SourceType>::type
    operator()(SourceType const& source, std::size_t source_bit_offset) const
    {
        return boost::make_unsigned<SourceType>::type(source) >>
               source_bit_offset;
    }

    template <typename SourceType>
    typename boost::make_unsigned<SourceType>::type
    operator()(SourceType const& source) const
    {
        return boost::make_unsigned<SourceType>::type(source);
    }

    std::size_t num_bits() const
    {
        return num_bits_;
    }

private:
    std::size_t num_bits_;
};

template <std::size_t Bits>
class static_bit_reader
{
public:
    template <typename SourceType>
    typename boost::make_unsigned<SourceType>::type
    operator()(SourceType const& source, std::size_t source_bit_offset) const
    {
        return boost::make_unsigned<SourceType>::type(source) >>
               source_bit_offset;
    }

    template <typename SourceType>
    typename boost::make_unsigned<SourceType>::type
    operator()(SourceType const& source) const
    {
        return boost::make_unsigned<SourceType>::type(source);
    }

    std::size_t num_bits() const
    {
        return Bits;
    }
};

}}} // namespace boost::radix::detail

#endif // BOOST_RADIX_DETAIL_IBITSTREAM_HPP
