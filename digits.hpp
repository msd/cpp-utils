#ifndef E045E175_73AB_47AF_A10F_AF45BCE24E41
#define E045E175_73AB_47AF_A10F_AF45BCE24E41

#include <algorithm>
#include <cmath>
#include <vector>

static inline int constexpr DECIMAL_BASE = 10;

std::vector<int> digits(int x)
{
    if (x < 0)
    {
        return digits(-x);
    }
    if (x == 0)
    {
        return {0};
    }
    std::vector<int> digits_;
    while (x > 0)
    {
        digits_.push_back(x % DECIMAL_BASE);
        x /= DECIMAL_BASE;
    }
#ifdef __cpp_lib_ranges
    std::ranges::reverse(digits_);
#else
    std::reverse(std::begin(digits), std::end(digits));
#endif
    return digits_;
}

auto digit_count(int x) { return static_cast<size_t>(std::log10(static_cast<float>(x) + 1)); }

auto digit_count_2(int x) { return digits(x).size(); }

#endif /* E045E175_73AB_47AF_A10F_AF45BCE24E41 */
