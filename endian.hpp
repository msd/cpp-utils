#ifndef A894F78F_41AC_4A45_91C7_243326655654
#define A894F78F_41AC_4A45_91C7_243326655654
// requires c++ 23

#include <algorithm>
#include <ranges>

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
    return le_copy_f<Value>(x, bytes_out);
}

template <trivially_copyable Value> inline auto to_big_endian(Value x, std::byte *bytes_out)
{
    return be_copy_f<Value>(x, bytes_out);
}

template <trivially_copyable Value, size_t Count = 1> struct bytes_array_for
{
    using type = std::array<std::byte, sizeof(Value) * Count>;
};
template <typename Value, size_t Count = 1>
using bytes_array_for_t = typename bytes_array_for<Value, Count>::type;

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
    requires(std::same_as<std::byte, typename std::iterator_traits<Iterator>::value_type>)
IteratorOut many_from_big_endian(Iterator beg, Sentinel end, IteratorOut result)
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
        *(result++) = from_big_endian<Value>(i);
    }
    return result;
}

// template <typename Tout, typename Iterator, typename Sentinel, typename IteratorOut>
template <trivially_copyable Value, std::input_iterator Iterator,
          std::sentinel_for<Iterator> Sentinel, std::output_iterator<Value> IteratorOut>
    requires(std::same_as<std::byte, typename std::iterator_traits<Iterator>::value_type>)
IteratorOut many_from_little_endian(Iterator beg, Sentinel end, IteratorOut result)
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
        *(result++) = from_little_endian<Value>(i);
    }
    return result;
}

template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel,
          std::output_iterator<std::byte> IteratorOut>
IteratorOut many_to_little_endian(Iterator beg, Sentinel end, IteratorOut result)
{
    while (beg != end)
    {
        result = to_little_endian(*beg, result);
        std::advance(beg, 1);
    }
    return result;
}

template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel, typename IteratorOut>
    requires(std::same_as<std::byte, typename std::iterator_traits<IteratorOut>::value_type>)
IteratorOut many_to_big_endian(Iterator beg, Sentinel end, IteratorOut result)
{
    while (beg != end)
    {
        result = to_big_endian(*beg, result);
        std::advance(beg, 1);
    }
    return result;
}

namespace ranges
{
    template <std::ranges::range Range, typename IteratorOut>
    auto constexpr many_to_big_endian(Range range, IteratorOut bytes_out)
    {
        return ::many_to_big_endian(std::ranges::cbegin(range), std::ranges::cend(range),
                                    bytes_out);
    }

    template <std::ranges::range Range, typename IteratorOut>
    auto constexpr many_to_little_endian(Range range, IteratorOut bytes_out)
    {
        return ::many_to_little_endian(std::ranges::cbegin(range), std::ranges::cend(range),
                                       bytes_out);
    }

    template <trivially_copyable Value, std::ranges::range Range,
              std::output_iterator<Value> IteratorOut>
    auto constexpr many_from_little_endian(Range range, IteratorOut result_values)
    {
        return ::many_from_little_endian<Value>(std::ranges::cbegin(range),
                                                std::ranges::cend(range), result_values);
    }

    template <trivially_copyable Value, std::ranges::range Range,
              std::output_iterator<Value> IteratorOut>
    auto constexpr many_from_big_endian(Range range, IteratorOut result_values)
    {
        return ::many_from_big_endian<Value>(std::ranges::cbegin(range), std::ranges::cend(range),
                                             result_values);
    }
} // namespace ranges

#endif /* A894F78F_41AC_4A45_91C7_243326655654 */
