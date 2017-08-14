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
#include <boost/static_assert.hpp>
#include <boost/assert.hpp>
#include <boost/align/align_up.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/make_unsigned.hpp>
#include <algorithm>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace radix { namespace detail {

/// \class ibitstream
template<typename InputIterator>
class ibitstream 
{
public:

    ibitstream(InputIterator first, InputIterator last)
        : current_(first)
        , end_(last)
        , bit_(0)
    {}

    template<typename ValueType, typename BitReader>
    void read_bits(ValueType& v, BitReader const& reader)
    {
        BOOST_ASSERT(num_bits <= sizeof(v) * 8);
        BOOST_ASSERT(num_bits > 0);

        typedef typename std::iterator_traits<InputIterator>::value_type buffer_element_type;

        // For now, we only support bytes.
        BOOST_STATIC_ASSERT(sizeof(buffer_element_type) == 1);

        std::size_t const element_bits = sizeof(buffer_element_type) * 8;

        // Extract whatever is left in the current element.
        v = reader(*current_, bit_);
        
        std::size_t bits_extracted = element_bits - bit_;
    
        if(bits_extracted == reader.read_size_bits())
        {
            ++current_;
            bit_ = 0;
            return;
        }
        else if(bits_extracted < reader.read_size_bits())
        {
            std::size_t bits_remaining = reader.read_size_bits() - bits_extracted;
            std::size_t aligned_bits_remaining = boost::alignment::align_up(bits_remaining, element_bits);
            std::size_t elements_remaining = aligned_bits_remaining / element_bits;
            BOOST_ASSERT(elements_remaining <= sizeof(v));
            
            // cglover-note: There's opportunity for optimization here if we know
            // the endiness of the machine.
            do
            {
                ++current_;
                ValueType rest = reader(*current_, 0);
                v |= rest << bits_extracted;
                bits_extracted += element_bits;
            } while(--elements_remaining);
        } 

        ValueType all_value_bit_mask = 0;
        all_value_bit_mask = ~all_value_bit_mask;

        // Mask off any overhang we didn't ask for.
        v &= ~(all_value_bit_mask << reader.read_size_bits());

        // Reset the bit offset to account for overhang.
        bit_ += reader.read_size_bits();
        while(bit_ >= 8)
        {
            bit_ -= 8;
        }

        if(bit_ == 0)
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
            typename std::iterator_traits<InputIterator>::value_type
        >::value
    );

    InputIterator current_;
    InputIterator end_;
    std::size_t bit_;
};

class msb_bit_reader
{
public:

    msb_bit_reader(std::size_t num_bits)
        : num_bits_(num_bits)
    {}

    template<typename SourceType>
    typename boost::make_unsigned<
        SourceType
    >::type operator()(SourceType const& source, std::size_t source_bit_offset) const
    {
        
    }

    std::size_t read_size_bits() const
    {
        return num_bits_;
    }

private:

    std::size_t num_bits_;
};

class lsb_bit_reader
{
public:

    lsb_bit_reader(std::size_t num_bits)
        : num_bits_(num_bits)
    {}

    template<typename SourceType>
    typename boost::make_unsigned<
        SourceType
    >::type operator()(SourceType const& source, std::size_t source_bit_offset) const
    {

    }

    std::size_t read_size_bits() const
    {
        return num_bits_;
    }

private:

    std::size_t num_bits_;
};

}}} // namespace boost { namespace radix { namespace detail {

#endif // BOOST_RADIX_DETAIL_IBITSTREAM_HPP