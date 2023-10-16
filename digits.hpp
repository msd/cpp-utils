#include <vector>
#include <cmath>
#include <algorithm>

std::vector<int> digits(int x)
{
    if (x < 0)
    {
        return digits(-x);
    }
    else if (x == 0)
    {
        return { 0 };
    }
    std::vector<int> digits_;
    while (x > 0)
    {
        digits_.push_back(x % 10);
        x /= 10;
    }
#ifdef __cpp_lib_ranges
    std::ranges::reverse(digits_);
#else
    std::reverse(std::begin(digits), std::end(digits));
#endif
    return digits_;
}

int digit_count(int x)
{
    return std::log10(static_cast<float>(x) + 1);
}

int digit_count_2(int x)
{
    return digits(x).size();
}
