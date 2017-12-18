//
// boost/radix/detail/segment_buffer.hpp
//
// Copyright (c) Chris Glover, 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_DETAIL_SEGMENTBUFFER_HPP
#define BOOST_RADIX_DETAIL_SEGMENTBUFFER_HPP

#include <boost/radix/common.hpp>

#include <boost/array.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#    pragma once
#endif

namespace boost { namespace radix { namespace detail {

template <typename ElementType, std::size_t Size>
class segment_buffer
{
public:
    typedef typename boost::array<ElementType, Size> container;
    typedef typename container::iterator iterator;
    typedef typename container::const_iterator const_iterator;
    typedef typename container::reference reference;
    typedef typename container::const_reference const_reference;

    segment_buffer()
        : size_(Size)
    {}

    reference operator[](std::size_t idx)
    {
        return container_[idx];
    }

    const_reference operator[](std::size_t idx) const
    {
        return container_[idx];
    }

    iterator begin()
    {
        return container_.begin();
    }

    iterator end()
    {
        return begin() + size();
    }

    const_iterator begin() const
    {
        return container_.begin();
    }

    const_iterator end() const
    {
        return begin() + size();
    }

    void pop_back()
    {
        --size_;
    }

    void resize(std::size_t size)
    {
        BOOST_ASSERT(size <= container_.size());
        size_ = size;
    }

    std::size_t size() const
    {
        return size_;
    }

private:
    container container_;
    std::size_t size_;
};

}}} // namespace boost::radix::detail

#endif // BOOST_RADIX_DETAIL_VARIABLEARRAY_HPP
