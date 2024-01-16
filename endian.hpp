#ifndef A894F78F_41AC_4A45_91C7_243326655654
#define A894F78F_41AC_4A45_91C7_243326655654
// requires c++ 23

#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <iterator>
#include <span>
#include <stdexcept>
#include <string>
#include <type_traits>

// #define OLDAPIENDIAN

template <typename T>
concept trivially_copyable = std::is_trivially_copyable_v<T>;

static_assert(trivially_copyable<int>);

// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast,
// cppcoreguidelines-pro-bounds-pointer-arithmetic)

template <trivially_copyable Value> inline auto copy_direct(Value x, std::byte *bytes_out)
{
    auto const bytes_in = reinterpret_cast<std::byte *>(&x);
    // return std::ranges::copy(std::span{bytes_in, sizeof x}, bytes_out);
    // return std::ranges::copy(std::ranges::subrange{bytes_in, bytes_in + sizeof x}, bytes_out);
    return std::copy(bytes_in, bytes_in + sizeof x, bytes_out);
}

template <trivially_copyable Value> inline auto copy_reverse(Value x, std::byte *bytes_out)
{
    auto const bytes_in = reinterpret_cast<std::byte *>(&x);
    // return std::ranges::reverse_copy(std::span{bytes_in, sizeof x}, bytes_out);
    // return std::ranges::reverse_copy(std::ranges::subrange{bytes_in, bytes_in + sizeof x},
    //                                  bytes_out);
    return std::reverse_copy(bytes_in, bytes_in + sizeof x, bytes_out);
}

template <trivially_copyable Value, std::input_iterator Iterator>
    requires(std::same_as<typename std::iterator_traits<Iterator>::value_type, std::byte>)
[[nodiscard]] inline auto load_direct(Iterator bytes_in)
{
    Value value{};
    auto bytes_out = reinterpret_cast<std::byte *>(&value);
    std::span<std::byte const> bytes(bytes_in, sizeof(Value));
    std::ranges::copy(bytes, bytes_out);
    return value;
}
template <trivially_copyable Value, std::input_iterator Iterator>
    requires(std::same_as<typename std::iterator_traits<Iterator>::value_type, std::byte>)
[[nodiscard]] inline auto load_reverse(Iterator bytes_in)
{
    Value value{};
    auto bytes_out = reinterpret_cast<std::byte *>(&value);
    std::reverse_copy(bytes_in, bytes_in + sizeof(Value), bytes_out);
    return value;
}

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast,
// cppcoreguidelines-pro-bounds-pointer-arithmetic)

template <trivially_copyable Int, std::endian> struct be_copy_s;
template <trivially_copyable Int> struct be_copy_s<Int, std::endian::little>
{
    static auto constexpr function = copy_reverse<Int>;
};
template <trivially_copyable Int> struct be_copy_s<Int, std::endian::big>
{
    static auto constexpr function = copy_direct<Int>;
};
template <typename Value>
auto const constexpr be_copy_f = be_copy_s<Value, std::endian::native>::function;

// Copy byte from value to an output iterator
template <trivially_copyable Int, std::endian> struct le_copy_s;
template <trivially_copyable Int> struct le_copy_s<Int, std::endian::little>
{
    static auto constexpr function = copy_direct<Int>;
};
template <trivially_copyable Int> struct le_copy_s<Int, std::endian::big>
{
    static auto constexpr function = copy_reverse<Int>;
};
template <typename Value>
auto const constexpr le_copy_f = le_copy_s<Value, std::endian::native>::function;

template <trivially_copyable Value> inline auto to_little_endian(Value x, std::byte *bytes_out)
{
    le_copy_f<Value>(x, bytes_out);
}

template <trivially_copyable Value> inline auto to_big_endian(Value x, std::byte *bytes_out)
{
    be_copy_f<Value>(x, bytes_out);
}

template <trivially_copyable Value> struct bytes_array_for
{
    using type = std::array<std::byte, sizeof(Value)>;
};
template <typename Value> using bytes_array_for_t = typename bytes_array_for<Value>::type;

template <trivially_copyable Value> [[nodiscard]] inline auto to_little_endian_array(Value x)
{
    bytes_array_for_t<Value> bytes_out{};
    to_little_endian(x, bytes_out.data());
    return bytes_out;
}
template <trivially_copyable Value> [[nodiscard]] inline auto to_big_endian_array(Value x)
{
    bytes_array_for_t<Value> bytes_out{};
    to_big_endian(x, bytes_out.data());
    return bytes_out;
}

// TEMPLATE SPECIALIZATION VERSION
// template <trivially_copyable Value, typename Iterator, std::endian> struct le_load_s;
// template <trivially_copyable Value, typename Iterator>
// struct le_load_s<Value, Iterator, std::endian::little>
// {
//     static auto const constexpr function = load_direct<Value, Iterator>;
// };
// template <trivially_copyable Value, typename Iterator>
// struct le_load_s<Value, Iterator, std::endian::big>
// {
//     static auto const constexpr function = load_reverse<Value, Iterator>;
// };
// template <trivially_copyable Value, typename Iterator>
// auto const constexpr le_load = le_load_s<Value, Iterator, std::endian::native>::function;

// template <std::endian, trivially_copyable Value, std::input_iterator Iterator> struct be_load_s;
// template <trivially_copyable Value, typename Iterator>
// struct be_load_s<std::endian::little, Value, Iterator>
// {
//     static auto const constexpr function = load_reverse<Value, Iterator>;
// };
// template <trivially_copyable Value, std::input_iterator Iterator>
// struct be_load_s<std::endian::big, Value, Iterator>
// {
//     static auto const constexpr function = load_direct<Value, Iterator>;
// };
// template <trivially_copyable Value, typename Iterator>
// auto const constexpr be_load = be_load_s<std::endian::native, Value, Iterator>::function;

// CONSTEXPR VERSION
template <trivially_copyable Value, typename Iterator> struct le_load_s
{
    static auto constexpr f()
    {
        if constexpr (std::endian::native == std::endian::little)
        {
            return load_direct<Value, Iterator>;
        }
        else
        {
            return load_reverse<Value, Iterator>;
        }
    }
    static auto const constexpr function = f();
};
template <trivially_copyable Value, typename Iterator>
auto const constexpr le_load = le_load_s<Value, Iterator>::function;

template <trivially_copyable Value, typename Iterator> struct be_load_s
{
    static auto constexpr f()
    {
        if constexpr (std::endian::native == std::endian::big)
        {
            return load_direct<Value, Iterator>;
        }
        else
        {
            return load_reverse<Value, Iterator>;
        }
    }
    static auto const constexpr function = f();
};
template <trivially_copyable Value, typename Iterator>
auto const constexpr be_load = be_load_s<Value, Iterator>::function;

template <trivially_copyable Value, std::input_iterator ByteIter>
[[nodiscard]] inline Value from_little_endian(ByteIter beg)
{
    return le_load<Value, ByteIter>(beg);
}

template <trivially_copyable Value>
[[nodiscard]] inline Value from_little_endian_array(bytes_array_for_t<Value> const &bytes)
{
    return le_load<Value, std::byte const *>(bytes.data());
}
template <trivially_copyable Value, std::input_iterator ByteIter>
// requires std::same_as<typename ByteIter::value_type, std::byte>
[[nodiscard]] inline Value from_big_endian(ByteIter beg)
{
    return be_load<Value, ByteIter>(beg);
}

template <trivially_copyable Value>
[[nodiscard]] inline Value from_big_endian_array(bytes_array_for_t<Value> const &bytes)
{
    return be_load<Value, std::byte const *>(bytes.data());
}

// template <typename Tout, typename Iterator, typename Sentinel, typename IteratorOut>
template <trivially_copyable Value, std::input_iterator Iterator,
          std::sentinel_for<Iterator> Sentinel, std::output_iterator<Value> IteratorOut>
IteratorOut many_from_big_endian(Iterator beg, Sentinel end, IteratorOut start)
{
    // decltype(std::iterator_traits<Iterator>::difference_type) byte_count = end - beg;
    auto byte_count = end - beg;
    size_t bytes_per_unit = sizeof(Value);
    if (byte_count % bytes_per_unit != 0)
    {
        throw std::runtime_error(
            "number of bytes given is not an integer multiple of the bytes per unit (count " +
            std::to_string(byte_count) + ", unit " + std::to_string(bytes_per_unit) + ")");
    }
    for (auto i = beg; i != end; std::advance(i, sizeof(Value)))
    {
        *(start++) = from_big_endian<Value>(i);
    }
    return start;
}

// template <typename Tout, typename Iterator, typename Sentinel, typename IteratorOut>
template <trivially_copyable Value, std::input_iterator Iterator,
          std::sentinel_for<Iterator> Sentinel, std::output_iterator<Value> IteratorOut>
IteratorOut many_from_little_endian(Iterator beg, Sentinel end, IteratorOut start)
{
    auto byte_count = end - beg;
    auto bytes_per_unit = sizeof(Value);
    if (byte_count % bytes_per_unit != 0)
    {
        throw std::runtime_error(
            "number of bytes given is not an integer multiple of the bytes per unit (count " +
            std::to_string(byte_count) + ", unit " + std::to_string(bytes_per_unit) + ")");
    }
    for (auto i = beg; i != end; std::advance(i, sizeof(Value)))
    {
        *(start++) = from_little_endian<Value>(i);
    }
    return start;
}

struct assertion_failed : public std::runtime_error
{
    explicit assertion_failed(std::string_view msg) : std::runtime_error{std::string{msg}} {}
};

// NOLINTBEGIN(*-magic-numbers)

namespace msd::endian::conversion::tests
{
    namespace my
    {
        static auto const asrt = [](bool x, std::string_view msg = "")
        {
            if (!x)
            {
                throw assertion_failed{msg};
            }
        };
    } // namespace my

    void endianess_conversions_test()
    {

        my::asrt(to_big_endian_array(static_cast<uint8_t>(0x11)) == std::array{std::byte{0x11}});
        my::asrt(to_big_endian_array(static_cast<uint16_t>(0x1122)) ==
                     std::array<std::byte, 2>{std::byte{0x11}, std::byte{0x22}},
                 "0x1122 little endian conversion ");
        my::asrt(to_big_endian_array(static_cast<uint32_t>(0x11223344)) ==
                 std::array{std::byte{0x11}, std::byte{0x22}, std::byte{0x33}, std::byte{0x44}});
        my::asrt(to_big_endian_array(static_cast<uint64_t>(0x1122334455667788ULL)) ==
                 std::array{std::byte{0x11}, std::byte{0x22}, std::byte{0x33}, std::byte{0x44},
                            std::byte{0x55}, std::byte{0x66}, std::byte{0x77}, std::byte{0x88}});

        my::asrt(to_little_endian_array(static_cast<uint8_t>(0x11)) == std::array{std::byte{0x11}});
        my::asrt(to_little_endian_array(static_cast<uint16_t>(0x1122)) ==
                     std::array<std::byte, 2>{std::byte{0x22}, std::byte{0x11}},
                 "0x1122 little endian conversion ");
        my::asrt(to_little_endian_array(static_cast<uint32_t>(0x11223344)) ==
                 std::array{std::byte{0x44}, std::byte{0x33}, std::byte{0x22}, std::byte{0x11}});
        my::asrt(to_little_endian_array(0x1122334455667788ULL) ==
                 std::array{std::byte{0x88}, std::byte{0x77}, std::byte{0x66}, std::byte{0x55},
                            std::byte{0x44}, std::byte{0x33}, std::byte{0x22}, std::byte{0x11}});

        my::asrt(from_big_endian_array<uint8_t>(std::array{std::byte{0x11}}) ==
                 static_cast<uint8_t>(0x11));
        my::asrt(from_big_endian_array<uint16_t>(std::array<std::byte, 2>{
                     std::byte{0x11}, std::byte{0x22}}) == static_cast<uint16_t>(0x1122),
                 "0x1122 little endian conversion ");
        my::asrt(from_big_endian_array<uint32_t>(std::array{std::byte{0x11}, std::byte{0x22},
                                                            std::byte{0x33}, std::byte{0x44}}) ==
                 static_cast<uint32_t>(0x11223344));
        my::asrt(from_big_endian_array<uint64_t>(
                     std::array{std::byte{0x11}, std::byte{0x22}, std::byte{0x33}, std::byte{0x44},
                                std::byte{0x55}, std::byte{0x66}, std::byte{0x77},
                                std::byte{0x88}}) == static_cast<uint64_t>(0x1122334455667788ULL));

        my::asrt(from_little_endian_array<uint8_t>(std::array{std::byte{0x11}}) ==
                 static_cast<uint8_t>(0x11));
        my::asrt(from_little_endian_array<uint16_t>(std::array<std::byte, 2>{
                     std::byte{0x22}, std::byte{0x11}}) == static_cast<uint16_t>(0x1122),
                 "0x1122 little endian conversion ");
        my::asrt(from_little_endian_array<uint32_t>(std::array{std::byte{0x44}, std::byte{0x33},
                                                               std::byte{0x22}, std::byte{0x11}}) ==
                 static_cast<uint32_t>(0x11223344));
        my::asrt(from_little_endian_array<uint64_t>(
                     std::array{std::byte{0x88}, std::byte{0x77}, std::byte{0x66}, std::byte{0x55},
                                std::byte{0x44}, std::byte{0x33}, std::byte{0x22},
                                std::byte{0x11}}) == static_cast<uint64_t>(0x1122334455667788ULL));
    }

} // namespace msd::endian::conversion::tests
// NOLINTEND(*-magic-numbers)

#endif /* A894F78F_41AC_4A45_91C7_243326655654 */
