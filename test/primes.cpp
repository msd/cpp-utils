#include <array>
#include <iostream>
#include <iterator>
#include <ranges>
#include <type_traits>

#include "../primes.hpp"
#include "test_case.hpp"

void run()
{
    msd::primes::FastPrimesIterator p{};
    size_t const constexpr chunk_size = 10'000;
    size_t const constexpr total_count = 1'000'000;
    std::array<unsigned, chunk_size> values{};
    auto it = p.begin();
    using it_cat = typename std::iterator_traits<
        typename std::array<std::string_view, 1>::iterator>::iterator_category;
    static_assert(std::is_base_of_v<std::input_iterator_tag, it_cat>);
    for (auto _ : std::ranges::views::iota(0ull, total_count / chunk_size))
    {
        (void)_;
        // for (auto &x : values)
        // {
        //     x = *p;
        //     ++p;
        // }
        std::copy_n(it, values.size(), values.begin());
        it += values.size();
    }
}

int main(int argc, char **argv)
{
    auto time = time_it(run);
    std::cout << static_cast<long double>(time) / 1.0e9l;
    return 0;
}
