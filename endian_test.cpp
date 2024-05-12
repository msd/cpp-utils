#include <concepts>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <ostream>
#include <ranges>
#include <stdexcept>
#include <vector>

#include "endian.hpp"

struct assertion_failed : public std::runtime_error
{
    explicit assertion_failed(std::string_view msg) : std::runtime_error{std::string{msg}} {}
};

// NOLINTBEGIN(*-magic-numbers)

namespace msd::endian::conversion::tests
{
    namespace my
    {
        static auto const asrt = [](bool x, std::string_view msg = "")
        {
            if (!x)
            {
                throw assertion_failed{msg};
            }
        };
    } // namespace my

    static void endianess_conversions_test()
    {
        my::asrt(to_big_endian_array(static_cast<uint8_t>(0x11)) == std::array{std::byte{0x11}});
        my::asrt(to_big_endian_array(static_cast<uint16_t>(0x1122)) ==
                     std::array<std::byte, 2>{std::byte{0x11}, std::byte{0x22}},
                 "0x1122 little endian conversion ");
        my::asrt(to_big_endian_array(static_cast<uint32_t>(0x11223344)) ==
                 std::array{std::byte{0x11}, std::byte{0x22}, std::byte{0x33}, std::byte{0x44}});
        my::asrt(to_big_endian_array(static_cast<uint64_t>(0x1122334455667788ULL)) ==
                 std::array{std::byte{0x11}, std::byte{0x22}, std::byte{0x33}, std::byte{0x44},
                            std::byte{0x55}, std::byte{0x66}, std::byte{0x77}, std::byte{0x88}});

        my::asrt(to_little_endian_array(static_cast<uint8_t>(0x11)) == std::array{std::byte{0x11}});
        my::asrt(to_little_endian_array(static_cast<uint16_t>(0x1122)) ==
                     std::array<std::byte, 2>{std::byte{0x22}, std::byte{0x11}},
                 "0x1122 little endian conversion ");
        my::asrt(to_little_endian_array(static_cast<uint32_t>(0x11223344)) ==
                 std::array{std::byte{0x44}, std::byte{0x33}, std::byte{0x22}, std::byte{0x11}});
        my::asrt(to_little_endian_array(0x1122334455667788ULL) ==
                 std::array{std::byte{0x88}, std::byte{0x77}, std::byte{0x66}, std::byte{0x55},
                            std::byte{0x44}, std::byte{0x33}, std::byte{0x22}, std::byte{0x11}});

        my::asrt(from_big_endian_array<uint8_t>(std::array{std::byte{0x11}}) ==
                 static_cast<uint8_t>(0x11));
        my::asrt(from_big_endian_array<uint16_t>(std::array<std::byte, 2>{
                     std::byte{0x11}, std::byte{0x22}}) == static_cast<uint16_t>(0x1122),
                 "0x1122 little endian conversion ");
        my::asrt(from_big_endian_array<uint32_t>(std::array{std::byte{0x11}, std::byte{0x22},
                                                            std::byte{0x33}, std::byte{0x44}}) ==
                 static_cast<uint32_t>(0x11223344));
        my::asrt(from_big_endian_array<uint64_t>(
                     std::array{std::byte{0x11}, std::byte{0x22}, std::byte{0x33}, std::byte{0x44},
                                std::byte{0x55}, std::byte{0x66}, std::byte{0x77},
                                std::byte{0x88}}) == static_cast<uint64_t>(0x1122334455667788ULL));

        my::asrt(from_little_endian_array<uint8_t>(std::array{std::byte{0x11}}) ==
                 static_cast<uint8_t>(0x11));
        my::asrt(from_little_endian_array<uint16_t>(std::array<std::byte, 2>{
                     std::byte{0x22}, std::byte{0x11}}) == static_cast<uint16_t>(0x1122),
                 "0x1122 little endian conversion ");
        my::asrt(from_little_endian_array<uint32_t>(std::array{std::byte{0x44}, std::byte{0x33},
                                                               std::byte{0x22}, std::byte{0x11}}) ==
                 static_cast<uint32_t>(0x11223344));
        my::asrt(from_little_endian_array<uint64_t>(
                     std::array{std::byte{0x88}, std::byte{0x77}, std::byte{0x66}, std::byte{0x55},
                                std::byte{0x44}, std::byte{0x33}, std::byte{0x22},
                                std::byte{0x11}}) == static_cast<uint64_t>(0x1122334455667788ULL));
    }

    template <std::input_iterator InputIterator1, std::sentinel_for<InputIterator1> Sentinel1,
              std::input_iterator InputIterator2, std::sentinel_for<InputIterator2> Sentinel2>
        requires(std::equality_comparable_with<
                 typename std::iterator_traits<InputIterator1>::value_type,
                 typename std::iterator_traits<InputIterator2>::value_type>)
    bool eq(InputIterator1 beg1, Sentinel1 end1, InputIterator2 beg2, Sentinel2 end2)
    {
        auto it1 = std::move(beg1);
        auto it2 = std::move(beg2);
        while (it1 != end1 && it2 != end2)
        {
            if (*it1 != *it2)
            {
                return false;
            }
            ++it1;
            ++it2;
        }
        if (it1 != end1 || it2 != end2)
        {
            std::cerr << "warning ranges to be tested for equality do not have the same length\n";
            return false;
        }
        return true;
    }

    template <std::ranges::range Range1, std::ranges::range Range2>
        requires(std::equality_comparable_with<std::ranges::range_value_t<Range1>,
                                               std::ranges::range_value_t<Range2>>)
    auto range_eq(Range1 range_1, Range2 range_2)
    {
        using namespace std::ranges;
        return eq(cbegin(range_1), cend(range_1), cbegin(range_2), cend(range_2));
    }

    template <std::ranges::range Range1, std::ranges::range Range2>
    void rng_asrt(Range1 range_1, Range2 range_2, std::string_view msg)
    {
        if (!range_eq(range_1, range_2))
        {
            throw assertion_failed(msg);
        }
    }

    static void ranges_test()
    {
        std::vector<std::byte> expected{std::byte{0x12}, std::byte{0x34}, std::byte{0x56},
                                        std::byte{0x78}};
        {
            bytes_array_for_t<uint32_t> got_le_1{};
            std::vector<uint32_t> data{0x78563412};
            ranges::many_to_little_endian(data, got_le_1.begin());
            rng_asrt(expected, got_le_1, "range le 1");
        }

        {
            bytes_array_for_t<uint16_t, 2> got_le_2{};
            std::array<uint16_t, 2> data{0x3412, 0x7856};
            ranges::many_to_little_endian(data, got_le_2.begin());
            rng_asrt(expected, got_le_2, "range le 2");
        }

        {
            bytes_array_for_t<uint32_t> got_be_1{};
            std::array<uint32_t, 1> data{0x12345678};
            ranges::many_to_big_endian(data, got_be_1.begin());
            rng_asrt(expected, got_be_1, "range be 1");
        }

        {
            bytes_array_for_t<uint16_t, 2> got_be_2{};
            std::array<uint16_t, 2> data{0x1234, 0x5678};
            ranges::many_to_big_endian(data, got_be_2.begin());
            rng_asrt(expected, got_be_2, "range be 2");
        }
    }

} // namespace msd::endian::conversion::tests

// NOLINTEND(*-magic-numbers)

int main()
{
    namespace tests = msd::endian::conversion::tests;

    try
    {
        tests::endianess_conversions_test();
        tests::ranges_test();
        std::cout << "ALL PASS\n";
        return 0;
    }
    catch (assertion_failed &ex)
    {
        std::cout << "FAILURE\n";
        std::cerr << ex.what() << std::endl;
    }
    return -1;
}
