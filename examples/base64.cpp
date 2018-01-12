#include <algorithm>
#include <boost/radix/codec/rfc4648/base64.hpp>
#include <boost/radix/decode.hpp>
#include <boost/radix/encode.hpp>
#include <iostream>
#include <vector>

int main()
{
    std::vector<unsigned char> data(32);
    std::generate(data.begin(), data.end(), []() {
        static int n = 'A';
        return n++;
    });

    std::cout << "Data: ";
    std::copy(
        data.begin(), data.end(),
        std::ostream_iterator<unsigned char>(std::cout));

    boost::radix::codec::rfc4648::base64 codec64;

    std::cout << "\n\nEncoded: ";
    boost::radix::encode(
        data.begin(), data.end(), std::ostream_iterator<char>(std::cout),
        codec64);

    std::vector<char> encoded;
    boost::radix::encode(
        data.begin(), data.end(), std::back_inserter(encoded), codec64);

    std::cout << "\n\nDecoded: ";
    boost::radix::decode(
        encoded.begin(), encoded.end(),
        std::ostream_iterator<unsigned char>(std::cout), codec64);
    return 0;
}