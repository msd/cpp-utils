cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(MsdUtils VERSION 0.0.4 LANGUAGES CXX)

set(MsdUtils_VERSION "${PROJECT_VERSION}")

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")
endif()

add_compile_options( -fmodules-ts -fdiagnostics-color=always)

add_executable(primes_test test/primes.cpp)

target_compile_features(primes_test PUBLIC cxx_std_23)

# Install and export the targets.
# install(FILES gromit.h DESTINATION include/gromit-${version})
# install(TARGETS gromit gromit-gen
#         DESTINATION lib/gromit-${version}
#         EXPORT gromit-targets)
# install(EXPORT gromit-targets
#         DESTINATION lib/gromit-${version})

configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/MsdUtilsConfig.cmake.in
    ${CMAKE_BINARY_DIR}/cmake/MsdUtilsConfig.cmake @ONLY
)
configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/MsdUtilsConfigVersion.cmake.in
    ${CMAKE_BINARY_DIR}/cmake/MsdUtilsConfigVersion.cmake @ONLY
)

add_library(MsdUtils INTERFACE
    primes.hpp endian.hpp endian_containers.hpp number_parser.hpp
    digits.hpp 
)

install(
    TARGETS MsdUtils
    DESTINATION "cmake"
    EXPORT MsdUtils-targets
)
install(
    EXPORT MsdUtils-targets
    DESTINATION "cmake"
)

install(
    TARGETS
        primes_test
    DESTINATION
        "test"
)
file(GLOB_RECURSE _headers RELATIVE ${CMAKE_SOURCE_DIR} "*.hpp")
install(
    FILES
        ${_headers}
    DESTINATION
        "include"
)
install(
    FILES 
        ${CMAKE_BINARY_DIR}/cmake/MsdUtilsConfig.cmake
        ${CMAKE_BINARY_DIR}/cmake/MsdUtilsConfigVersion.cmake
    DESTINATION
        "cmake"
)
