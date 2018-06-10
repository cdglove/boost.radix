//
// boost/radix/encode_iterator.hpp
//
// Copyright (c) Chris Glover, 2017-2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_RADIX_ENCODEITERATOR_HPP
#define BOOST_RADIX_ENCODEITERATOR_HPP

#include <boost/radix/common.hpp>

#include <boost/radix/encode.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <iterator>

namespace boost { namespace radix {
// -----------------------------------------------------------------------------
//
template <typename Codec, typename InnerIterator>
class encode_iterator
    : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
public:
    encode_iterator(Codec const& codec, InnerIterator iter)
        : encoder_(boost::make_shared<encoder<Codec, InnerIterator> >(codec, iter))
    {}

    encode_iterator& operator++()
    {
        return *this;
    }

    encode_iterator& operator++(int)
    {
        return *this;
    }

    encode_iterator& operator*()
    {
        return *this;
    }

    template <typename T>
    encode_iterator& operator=(T const& t)
    {
        encoder_->append(&t, (&t) + 1);
        return *this;
    }

    void flush()
    {
        encoder_->flush();
    }

private:
    boost::shared_ptr<encoder<Codec, InnerIterator> > encoder_;
};

template <typename Codec, typename InnerIterator>
encode_iterator<Codec, InnerIterator>
make_encode_iterator(Codec const& codec, InnerIterator iter)
{
    return encode_iterator<Codec, InnerIterator>(codec, iter);
}

}} // namespace boost::radix

#endif // BOOST_RADIX_ENCODEITERATOR_HPP
