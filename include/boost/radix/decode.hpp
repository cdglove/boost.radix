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

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace radix { 

template<typename Codec>
std::size_t decoded_size(std::size_t encoded_size, Codec const& codec)
{
    return codec.decoded_size(encoded_size);
}

template<typename InputIterator, typename OutputIterator, typename Codec>
void decode(InputIterator first, InputIterator last,
            OutputIterator out, Codec const& codec) 
{
    unsigned char output_buffer[3];
    unsigned char input_buffer[4];

    auto const& alpha = codec;
    int i = 0;

    

    while(first != last)
    {
    	unsigned char current = *first++;
        if(current == codec.get_pad_char())
            break;

        input_buffer[i++] = current;
        if (i == 4)
        {
            for (i = 0; i < 4; ++i)
                input_buffer[i] = alpha.bits_from_char(input_buffer[i]);

            output_buffer[0] = (input_buffer[0] << 2) + ((input_buffer[1] & 0x30) >> 4);
            output_buffer[1] = ((input_buffer[1] & 0xf) << 4) + ((input_buffer[2] & 0x3c) >> 2);
            output_buffer[2] = ((input_buffer[2] & 0x3) << 6) + input_buffer[3];

            for (i = 0; (i < 3); ++i)
                *out++ = output_buffer[i];
            i = 0;
        }
    }

    if (i)
    {
        for (int j = i; j < 4; ++j)
            input_buffer[j] = 0;

        for (int j = 0; j < 4; ++j)
            input_buffer[j] = alpha.bits_from_char(input_buffer[j]);

        output_buffer[0] = (input_buffer[0] << 2) + ((input_buffer[1] & 0x30) >> 4);
        output_buffer[1] = ((input_buffer[1] & 0xf) << 4) + ((input_buffer[2] & 0x3c) >> 2);
        output_buffer[2] = ((input_buffer[2] & 0x3) << 6) + input_buffer[3];

        for (int j = 0; (j < i - 1); ++j)
            *out++ = output_buffer[j];
    }
}

}} // namespace boost { namespace radix { 

#endif // BOOST_RADIX_DECODE_HPP