# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


## Unreleased [[0.0.3]]
    Fixed bug when including multiple times 
    Added many_to_* conversion

## [[0.0.2]] 2024/01/01
    Added little_endian_decoder similar to big_endian_decoder
    Fixed missing include guard in digits.hpp
    Added conanfile.py to allow for creating a conan package
    Added changelog
    Added README.md
    Added .clangd to provide configuration for the clangd LSP linter
    Added .clang-format to provide configuration for clang-format source code beautifier

## [[0.0.1]] - 2023/10/16 - Initial version 
    Added endian.hpp for converting between endianess (undefined bahavior town)
    Added digits.hpp just for splitting an integer into its constituent digits
    Added number_parser.hpp for parsing a string to an optional number
