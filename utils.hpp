#ifndef B9B475FE_C6E6_4A9F_98B5_A506D530305D
#define B9B475FE_C6E6_4A9F_98B5_A506D530305D

#include <iomanip>
#include <ranges>
#include <sstream>
#include <string>

namespace msd::utils
{
    template <typename F, typename X> X iterate(F f, X x, size_t count)
    {
        for (size_t i = 0; i != count; i++)
        {
            x = f(x);
        }
        return x;
    }

    template <typename... Args> auto constexpr make_array(Args... args)
    {
        return std::array<std::common_type_t<Args...>, sizeof...(Args)>{args...};
    }

    struct hex_bytes_params
    {
        unsigned split = 0;
    };

    // byte count of array
    template <typename Array> struct array_byte_size
    {
        static size_t const constexpr value =
            std::tuple_size<Array>() * sizeof(typename Array::value_type);
    };

    template <typename Array>
    auto const constexpr array_byte_size_v = array_byte_size<Array>::value;

    template <typename Integer> std::string to_hex(Integer x)
    {
        static auto const new_stream = []
        {
            std::stringstream the_new_stream;
            the_new_stream << std::right << std::hex << std::setfill('0');
            return the_new_stream;
        };

        auto stream = new_stream();
        stream << std::setw(sizeof(Integer) * 2) << static_cast<size_t>(x);
        return stream.str();
    }

    template <typename Container>
        requires(
            std::same_as<std::byte, std::remove_cvref_t<std::ranges::range_value_t<Container>>>)
    std::string many_bytes_to_hex(Container container, unsigned split = 0)
    {
        namespace views = std::ranges::views;
        auto const hexes = container | views::transform(to_hex<std::byte>);
        std::stringstream stream;
        if (split == 0)
        {
            for (auto hex : hexes)
            {
                stream << hex;
            }
            return stream.str();
        }

        for (auto group : hexes | views::chunk(split))
        {
            for (auto hex : group)
            {
                stream << hex;
            }
            stream << ' ';
        }
        auto x = stream.str();
        // todo remove last character from stream instead of taking substr
        return x.substr(0, x.size() - 1);
    }
} // namespace msd::utils

#endif /* B9B475FE_C6E6_4A9F_98B5_A506D530305D */
