#ifndef A894F78F_41AC_4A45_91C7_243326655654
#define A894F78F_41AC_4A45_91C7_243326655654
// requires c++ 23

#include <algorithm>
#include <iterator>
#include <span>
#include <type_traits>

// #define OLDAPIENDIAN

namespace msd::utils::endian
{
    template <typename T>
    concept trivially_copyable = std::is_trivially_copyable_v<T>;

    static_assert(trivially_copyable<int>);

    template <trivially_copyable Value, std::output_iterator<std::byte> OutputIterator>
    inline auto constexpr dump_direct(Value x, OutputIterator bytes_out)
    {
        auto const bytes_in = std::as_bytes(std::span<Value const, 1>{&x, 1});
        return std::copy(std::cbegin(bytes_in), std::cend(bytes_in), bytes_out);

        // auto const bytes_in = reinterpret_cast<std::byte *>(&x);
        // return std::copy(bytes_in, bytes_in + sizeof x, bytes_out);

        // auto const bytes_in = reinterpret_cast<std::byte *>(&x);
        // std::ranges::copy(std::span<std::byte const, sizeof x>{bytes_in, sizeof x}, bytes_out);
        // return std::next(bytes_out, sizeof x);

        // for (std::byte const &byte : std::as_bytes(std::span<Value const, 1>{&x, 1}))
        // {
        //     *bytes_out = byte;
        //     std::advance(bytes_out, 1);
        // }
        // return bytes_out;
    }

    template <trivially_copyable Value, std::output_iterator<std::byte> OutputIterator>
    inline auto constexpr dump_reverse(Value x, OutputIterator bytes_out)
    {
        auto const bytes_in = std::as_bytes(std::span<Value const, 1>{&x, 1});
        return std::reverse_copy(std::cbegin(bytes_in), std::cend(bytes_in), bytes_out);

        // auto const bytes_in = reinterpret_cast<std::byte *>(&x);
        // return std::reverse_copy(bytes_in, bytes_in + sizeof x, bytes_out);

        // auto const bytes_in = reinterpret_cast<std::byte *>(&x);
        // std::ranges::reverse_copy(std::span<std::byte const, sizeof x>{bytes_in, sizeof x},
        //                           bytes_out);
        // return std::next(bytes_out, sizeof x);

        // auto bytes_in = std::as_bytes(std::span<Value const, 1>{&x, 1});
        // auto const bytes_in_beg = std::cbegin(bytes_in);
        // auto const bytes_in_end = std::cend(bytes_in);
        // auto bytes_in_it = bytes_in_end;
        // while (bytes_in_it != bytes_in_beg)
        // {
        //     std::advance(bytes_in_it, -1);
        //     *bytes_out = *bytes_in_it;
        //     std::advance(bytes_out, 1);
        // }
        // return bytes_out;
    }

    template <trivially_copyable Value, std::input_iterator BytesInputIterator>
        requires(std::same_as<std::iter_value_t<BytesInputIterator>, std::byte>)
    [[nodiscard]] inline auto load_direct(BytesInputIterator bytes_in)
    {
        Value value{};
        // auto bytes_out = reinterpret_cast<std::byte *>(&value);
        auto bytes_out = std::as_writable_bytes(std::span<Value, 1>{&value, 1});
        std::copy(bytes_in, std::next(bytes_in, sizeof(Value)), std::begin(bytes_out));
        return value;
    }
    template <trivially_copyable Value, std::input_iterator BytesInputIterator>
        requires(
            std::same_as<std::remove_cvref_t<std::iter_value_t<BytesInputIterator>>, std::byte>)
    [[nodiscard]] inline auto load_reverse(BytesInputIterator bytes_in)
    {
        Value value{};
        auto bytes_out = std::as_writable_bytes(std::span<Value, 1>{&value, 1});
        std::reverse_copy(bytes_in, std::next(bytes_in, sizeof(Value)), std::begin(bytes_out));
        return value;
    }

    template <trivially_copyable Int, std::output_iterator<std::byte> BytesOutputIterator,
              std::endian>
    struct be_dump_s;
    template <trivially_copyable Int, std::output_iterator<std::byte> BytesOutputIterator>
    struct be_dump_s<Int, BytesOutputIterator, std::endian::little>
    {
        static auto constexpr function = dump_reverse<Int, BytesOutputIterator>;
    };
    template <trivially_copyable Int, std::output_iterator<std::byte> BytesOutputIterator>
    struct be_dump_s<Int, BytesOutputIterator, std::endian::big>
    {
        static auto constexpr function = dump_direct<Int, BytesOutputIterator>;
    };
    template <typename Value, typename BytesOutputIterator>
    auto const constexpr be_copy_f =
        be_dump_s<Value, BytesOutputIterator, std::endian::native>::function;

    // Copy byte from value to an output iterator
    template <trivially_copyable Value, std::output_iterator<std::byte> ByteOutputIterator,
              std::endian>
    struct le_dump_s;
    template <trivially_copyable Value, std::output_iterator<std::byte> ByteOutputIterator>
    struct le_dump_s<Value, ByteOutputIterator, std::endian::little>
    {
        static auto constexpr function = dump_direct<Value, ByteOutputIterator>;
    };
    template <trivially_copyable Value, std::output_iterator<std::byte> ByteOutputIterator>
    struct le_dump_s<Value, ByteOutputIterator, std::endian::big>
    {
        static auto constexpr function = dump_reverse<Value, ByteOutputIterator>;
    };
    template <typename Value, typename OutputIterator>
    auto const constexpr le_copy_f =
        le_dump_s<Value, OutputIterator, std::endian::native>::function;

    template <trivially_copyable Value, std::output_iterator<std::byte> BytesOutputIterator>
    inline auto to_little_endian(Value x, BytesOutputIterator bytes_out)
    {
        return le_copy_f<Value, BytesOutputIterator>(x, bytes_out);
    }

    template <trivially_copyable Value, std::output_iterator<std::byte> BytesOutputIterator>
    inline auto to_big_endian(Value x, BytesOutputIterator bytes_out)
    {
        return be_copy_f<Value, BytesOutputIterator>(x, bytes_out);
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

    // template <std::endian, trivially_copyable Value, std::input_iterator Iterator> struct
    // be_load_s; template <trivially_copyable Value, typename Iterator> struct
    // be_load_s<std::endian::little, Value, Iterator>
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
    template <trivially_copyable Value, typename BytesInputIterator> struct le_load_s
    {
        static auto constexpr f()
        {
            if constexpr (std::endian::native == std::endian::little)
            {
                return load_direct<Value, BytesInputIterator>;
            }
            else
            {
                return load_reverse<Value, BytesInputIterator>;
            }
        }
        static auto const constexpr function = f();
    };
    template <trivially_copyable Value, typename BytesInputIterator>
    auto const constexpr le_load = le_load_s<Value, BytesInputIterator>::function;

    template <trivially_copyable Value, typename BytesInputIterator> struct be_load_s
    {
        static auto constexpr f()
        {
            if constexpr (std::endian::native == std::endian::big)
            {
                return load_direct<Value, BytesInputIterator>;
            }
            else
            {
                return load_reverse<Value, BytesInputIterator>;
            }
        }
        static auto const constexpr function = f();
    };
    template <trivially_copyable Value, typename ByteInputIterator>
    auto const constexpr be_load = be_load_s<Value, ByteInputIterator>::function;

    template <trivially_copyable Value, std::input_iterator ByteInputIterator>
    [[nodiscard]] inline Value from_little_endian(ByteInputIterator beg)
    {
        return le_load<Value, ByteInputIterator>(beg);
    }

    template <trivially_copyable Value, std::input_iterator BytesInputIterator>
    // requires std::same_as<typename ByteIter::value_type, std::byte>
    [[nodiscard]] inline Value from_big_endian(BytesInputIterator beg)
    {
        return be_load<Value, BytesInputIterator>(beg);
    }

    // template <typename Tout, typename Iterator, typename Sentinel, typename IteratorOut>
    template <trivially_copyable Value, std::input_iterator ByteIterator,
              std::sentinel_for<ByteIterator> ByteSentinel, std::output_iterator<Value> IteratorOut>
        requires(std::same_as<std::byte, std::remove_cvref_t<std::iter_value_t<ByteIterator>>>)
    IteratorOut many_from_big_endian(ByteIterator beg, ByteSentinel end, IteratorOut result)
    {
        auto const byte_count = std::distance(beg, end);
        size_t bytes_per_unit = sizeof(Value);
        if (byte_count % bytes_per_unit != 0)
        {
            throw std::runtime_error(
                "number of bytes given is not an integer multiple of the bytes per unit (count " +
                std::to_string(byte_count) + ", unit " + std::to_string(bytes_per_unit) + ")");
        }
        for (auto i = beg; i != end; std::advance(i, sizeof(Value)), std::advance(result, 1))
        {
            *result = from_big_endian<Value>(i);
        }
        return result;
    }

    // template <typename Tout, typename Iterator, typename Sentinel, typename IteratorOut>
    template <trivially_copyable Value, std::input_iterator Iterator,
              std::sentinel_for<Iterator> Sentinel, std::output_iterator<Value> IteratorOut>
        requires(std::same_as<std::byte, std::iter_value_t<Iterator>>)
    IteratorOut many_from_little_endian(Iterator beg, Sentinel end, IteratorOut result)
    {
        auto const byte_count = std::distance(beg, end);
        auto bytes_per_unit = sizeof(Value);
        if (byte_count % bytes_per_unit != 0)
        {
            throw std::runtime_error(
                "number of bytes given is not an integer multiple of the bytes per unit (count " +
                std::to_string(byte_count) + ", unit " + std::to_string(bytes_per_unit) + ")");
        }
        for (auto i = beg; i != end; std::advance(i, sizeof(Value)), std::advance(result, 1))
        {
            *result = from_little_endian<Value>(i);
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

    template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel,
              std::output_iterator<std::byte> IteratorOut>
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
        auto constexpr many_to_big_endian(Range range, IteratorOut result_bytes)
        {
            return msd::utils::endian::many_to_big_endian(std::ranges::cbegin(range),
                                                          std::ranges::cend(range), result_bytes);
        }

        template <std::ranges::range Range, typename IteratorOut>
        auto constexpr many_to_little_endian(Range range, IteratorOut result_bytes)
        {
            return msd::utils::endian::many_to_little_endian(
                std::ranges::cbegin(range), std::ranges::cend(range), result_bytes);
        }

        template <trivially_copyable Value, std::ranges::range Range,
                  std::output_iterator<Value> IteratorOut>
        auto constexpr many_from_little_endian(Range range, IteratorOut result_values)
        {
            return msd::utils::endian::many_from_little_endian<Value>(
                std::ranges::cbegin(range), std::ranges::cend(range), result_values);
        }

        template <trivially_copyable Value, std::ranges::range Range,
                  std::output_iterator<Value> IteratorOut>
        auto constexpr many_from_big_endian(Range range, IteratorOut result_values)
        {
            return msd::utils::endian::many_from_big_endian<Value>(
                std::ranges::cbegin(range), std::ranges::cend(range), result_values);
        }
    } // namespace ranges

} // namespace msd::utils::endian

#endif /* A894F78F_41AC_4A45_91C7_243326655654 */
