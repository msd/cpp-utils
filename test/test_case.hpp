#ifndef B84E43F0_B93A_400A_9064_9FBB3B76BD92
#define B84E43F0_B93A_400A_9064_9FBB3B76BD92

#include <chrono>
#include <string_view>

auto time_now() { return std::chrono::system_clock::now().time_since_epoch().count(); }

template <std::invocable F> auto time_it(F f)
{
    auto start_time = time_now();
    f();
    return time_now() - start_time;
}

struct test_case
{
    std::string_view case_name;
    unsigned test_count;
    unsigned success_count;
};

#endif /* B84E43F0_B93A_400A_9064_9FBB3B76BD92 */
