# boost.radix

Note: Boost.Radix is **not** an offical boost library. It is written in a boost style, and placed in the boost namespace because I hope to have the library encompassed in boost in the future.

Boost.Radix is a library that aims to generalise conversion between binary and text. Common examples of such encodings are base64, hex, binary, etc. There are many implementations ofr such libraries, but none are configurable to suit the user. With this, the aims of boost.radix are.

- Supply a general mechanism for users to convert between binary and text data
- Supply a toolkit to facilitate construction of different encoding strategies
- Supply some default encodings (base64, hex, etc) using the toolkit.
- Allow the user to customise the mechanism
  - Custom bit interpretation
  - Custom alphabets
  - Custom whitespace handling
  - Custom error handling
- Library should be fast -- aim for zero overhead abstraction
- Library should be easy to use

# Example

```cpp
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

    std::cout << "Data:\n";
    std::copy(
        data.begin(), data.end(),
        std::ostream_iterator<unsigned char>(std::cout));

    boost::radix::codec::rfc4648::base64 codec64;

    std::cout << "\n\nEncoded:\n";
    boost::radix::encode(
        data.begin(), data.end(), std::ostream_iterator<char>(std::cout),
        codec64);

    std::vector<char> encoded;
    boost::radix::encode(
        data.begin(), data.end(), std::back_inserter(encoded), codec64);

    std::cout << "\n\nDecoded:\n";
    boost::radix::decode(
        encoded.begin(), encoded.end(),
        std::ostream_iterator<unsigned char>(std::cout), codec64);
    return 0;
}
```

###### Output

> Data:
> ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`
>
> Encoded:
> QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVpbXF1eX2A=
>
> Decoded:
> ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`
