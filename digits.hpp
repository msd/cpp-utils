#ifndef E045E175_73AB_47AF_A10F_AF45BCE24E41
#define E045E175_73AB_47AF_A10F_AF45BCE24E41

#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <optional>
#include <ranges>
#include <type_traits>
#include <vector>

static int const constexpr DECIMAL_BASE = 10;

namespace msd::utils
{
    template <std::unsigned_integral Int> struct digits_view
    {
        struct iterator
        {
            using value_type = const int;
            using reference = const int &;
            using iterator_category = std::input_iterator_tag;
            using difference_type = ptrdiff_t; // not actually used lol
            // using pointer = void;

            Int num;
            difference_type index = 0;
            std::optional<Int> div_cache = {};
            int digit;

            [[nodiscard]] auto div_calc() const
            {
                auto result = num;
                for (auto const &_ : std::ranges::views::iota(0ll, index))
                {
                    (void)_;
                    result /= DECIMAL_BASE;
                }
                return result;
            }

            [[nodiscard]] auto div()
            {
                if (!div_cache)
                {
                    div_cache = div_calc();
                }
                return *div_cache;
            }

            [[nodiscard]] auto div() const
            {
                if (!div_cache)
                {
                    return div_calc();
                }
                return *div_cache;
            }

            [[nodiscard]] int mod() { return static_cast<int>(div() % DECIMAL_BASE); }

            [[nodiscard]] int mod() const { return static_cast<int>(div() % DECIMAL_BASE); }

            explicit constexpr iterator(Int const &n) : num{n}, digit(mod()) {}

            [[nodiscard]] explicit constexpr operator bool() { return first() || div() > 0; }

            [[nodiscard]] explicit constexpr operator bool() const { return first() || div() > 0; }

            [[nodiscard]] auto const constexpr &operator*() const { return digit; }

            auto constexpr first() const { return index == 0; }

            auto constexpr operator-(iterator const &other)
            {
                return static_cast<difference_type>(index - other.index);
            }

            auto constexpr &operator+=(difference_type di)
            {
                index += di;
                digit = mod();
                return *this;
            }

            auto constexpr &operator-=(difference_type di) { return *this += (-di); }

            auto constexpr &operator++() { return *this += 1; }

            iterator constexpr operator++(int)
            {
                iterator tmp{*this};
                ++(*this);
                return tmp;
            }

            auto constexpr &operator--() { return *this -= 1; }

            auto constexpr operator--(int)
            {
                iterator tmp{*this};
                --(*this);
                return tmp;
            }
        };

        struct sentinel
        {
            [[nodiscard]] bool constexpr operator==(iterator const &it) const
            {
                return static_cast<bool>(it);
            }
        };

        Int num;
        explicit digits_view(Int x) : num{x} {}

        auto constexpr cbegin() { return iterator{num}; }

        auto constexpr cend() { return sentinel{}; }

        auto constexpr begin() { return iterator{num}; }
        auto constexpr end() { return sentinel{}; }
    };

    static_assert(std::input_iterator<digits_view<unsigned>::iterator>);

    template <std::integral Int> auto constexpr abs_if_necessary(Int x)
    {
        if constexpr (std::is_signed_v<Int>)
        {
            // lol undefined if x is unsigned
            return static_cast<std::make_unsigned_t<Int>>(std::abs(x));
        }
        else
        {
            return x;
        }
    }

    namespace views
    {
        template <std::integral Int> auto constexpr digits(Int x)
        {
            auto v = abs_if_necessary(x);
            return digits_view<decltype(v)>{v};
        }
    } // namespace views
} // namespace msd::utils

template <std::integral Int> std::vector<int> digits(Int x)
{
    if constexpr (std::is_signed_v<Int>)
    {
        if (x < 0)
        {
            return digits(-x);
        }
    }
    if (x == 0)
    {
        return {0};
    }
    std::vector<int> digits_;
    while (x > 0)
    {
        digits_.emplace_back(x % DECIMAL_BASE);
        x /= DECIMAL_BASE;
    }
#ifdef __cpp_lib_ranges
    std::ranges::reverse(digits_);
#else
    std::reverse(digits.begin(), digits.end());
#endif
    return digits_;
}

auto digit_count(int x) { return static_cast<size_t>(std::log10(static_cast<float>(x) + 1)); }

auto digit_count_2(int x) { return digits(x).size(); }

#endif /* E045E175_73AB_47AF_A10F_AF45BCE24E41 */
