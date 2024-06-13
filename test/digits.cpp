#include <iostream>
#include <iterator>
#include <minwindef.h>
#include <ranges>

#include "../digits.hpp"

using dig_view = msd::utils::digits_view<unsigned long>;

static_assert(std::input_iterator<dig_view::iterator>);
static_assert(std::sentinel_for<dig_view::sentinel, dig_view::iterator>);
static_assert(std::ranges::range<dig_view>);
static_assert(std::same_as<std::ranges::range_value_t<dig_view>, int>);

int main()
{
    int x = 123;
    std::cout << x << "\ndigits: ";
    for (auto d : msd::utils::views::digits(x))
    {
        std::cout << d << ' ';
    }
    std::cout << "\ndone" << std::endl;
    return 0;
}
