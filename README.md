# Utilities for C++
These utilities are ment for private use but someone may find a use for them.
Currently these are header-only so you can just copy them into your project and begin using them right away.

Source code formatting: `clang-format` configuration provided (`/.clang-format`)

## Documentation

### `endian.hpp`
Convert to and from little/big endian without having to deal with the system's endianess.
These are mostly undefined behavior because they use type punning (gasp).
Regardless they do work on the tested compilers but YMMV.

Compilers tested: gcc, msvc

### `digits.hpp`
Split a number into its constituent decimal digits.

### `number_parser.hpp`
Convert strings to numbers using `std::optional` without dealing with exceptions.
