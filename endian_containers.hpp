#ifndef EAC285BC_437B_4795_BF08_10151FA1E4AE
#define EAC285BC_437B_4795_BF08_10151FA1E4AE

#include <iterator>
#include <vector>

#include "endian.hpp"

namespace msd::utils::endian::containers
{
    template <trivially_copyable Value, size_t Count = 1> struct bytes_array_for
    {
        using type = std::array<std::byte, sizeof(Value) * Count>;
    };
    template <typename Value, size_t Count = 1>
    using bytes_array_for_t = typename bytes_array_for<Value, Count>::type;

    template <trivially_copyable Value> [[nodiscard]] inline auto to_big_endian_array(Value x)
    {
        bytes_array_for_t<Value> bytes_out{};
        to_big_endian(x, bytes_out.begin());
        return bytes_out;
    }

    template <trivially_copyable Value> [[nodiscard]] inline auto to_little_endian_array(Value x)
    {
        bytes_array_for_t<Value> bytes_out{};
        to_little_endian(x, bytes_out.begin());
        return bytes_out;
    }

    template <trivially_copyable Value>
    [[nodiscard]] inline Value from_big_endian_array(bytes_array_for_t<Value> const &bytes)
    {
        return from_big_endian<Value>(bytes.cbegin());
    }

    template <trivially_copyable Value>
    [[nodiscard]] inline Value from_little_endian_array(bytes_array_for_t<Value> const &bytes)
    {
        return from_little_endian<Value>(bytes.cbegin());
    }

    template <typename Value, std::input_iterator InputIterator,
              std::sentinel_for<InputIterator> InputSentinel>
    [[nodiscard]] inline auto many_from_big_endian_vector(InputIterator beg, InputSentinel end)
    {
        std::vector<Value> results;
        many_from_big_endian<Value>(beg, end, std::back_inserter(results));
        return results;
    }

    template <typename Value, std::input_iterator InputIterator,
              std::sentinel_for<InputIterator> InputSentinel>
    [[nodiscard]] inline auto many_from_little_endian_vector(InputIterator beg, InputSentinel end)
    {
        std::vector<Value> results;
        many_from_little_endian<Value>(beg, end, std::back_inserter(results));
        return results;
    }

    template <std::input_or_output_iterator ValueIterator,
              std::sentinel_for<ValueIterator> ValueSentinel>
    [[nodiscard]] inline auto many_to_big_endian_vector(ValueIterator beg, ValueSentinel end)
    {
        std::vector<std::byte> results;
        many_to_big_endian(beg, end, std::back_inserter(results));
        return results;
    }

    template <std::input_or_output_iterator ValueIterator,
              std::sentinel_for<ValueIterator> ValueSentinel>
    [[nodiscard]] inline auto many_to_little_endian_vector(ValueIterator beg, ValueSentinel end)
    {
        std::vector<std::byte> results;
        many_to_little_endian(beg, end, std::back_inserter(results));
        return results;
    }
} // namespace msd::utils::endian::containers

#endif /* EAC285BC_437B_4795_BF08_10151FA1E4AE */
