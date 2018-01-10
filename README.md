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

```
#include <boost/radix/encode.hpp>
#include <boost/radix/codec/rfc4648/base64.hpp>

int main()
{
    boost::radix::codec::rfc4648::base64 codec64;
    boost::radix::encode(buf.begin(), buf.end(), std::ostream_iterator<char>(std::cout), codec64);
    return 0;
}
