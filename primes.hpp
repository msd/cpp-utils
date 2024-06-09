#ifndef BB16AA93_3809_4DDD_9077_CD9E457D9E6C
#define BB16AA93_3809_4DDD_9077_CD9E457D9E6C

#include <array>
#include <corecrt.h>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <vector>

namespace msd::primes
{
    template <class Integer> class PrimeFinderBase
    {
    public:
        struct iterator
        {
            using value_type = Integer;
            using difference_type = ptrdiff_t;
            using iterator_category = std::input_iterator_tag;
            using reference = Integer &;
            size_t index = 0;
            PrimeFinderBase *self;
            value_type value;

            constexpr iterator(PrimeFinderBase *self_)
                : self{self_}, value{self->discovered.front()}
            {
            }

            auto constexpr &operator+=(size_t n)
            {
                index += n;
                value = self->get(index);
                return *this;
            }
            auto constexpr operator+(size_t n) const { return iterator{*this} += n; }

            auto constexpr &operator-=(ptrdiff_t n)
            {
                index -= n;
                return *this;
            }
            [[nodiscard]] auto constexpr operator-(ptrdiff_t n) const
            {
                return iterator{*this} -= n;
            }

            [[nodiscard]] auto constexpr operator-(iterator const &other) const
            {
                return static_cast<ptrdiff_t>(index) - static_cast<ptrdiff_t>(other.index);
            }

            // prefix
            auto constexpr &operator++() { return (*this) += 1; }
            // postfix
            auto constexpr operator++(int)
            {
                iterator tmp{*this};
                ++(*this);
                return tmp;
            }

            auto &operator*() { return self->get(index); }
            auto const &operator*() const { return self->get(index); }
        };

        struct sentinel
        {
            auto constexpr &operator+=(size_t n) { return *this; }
            [[nodiscard]] auto constexpr operator+(size_t n) const { return *this; }

            // prefix
            auto constexpr &operator++() { return (*this) += 1; }
            // postfix
            auto constexpr operator++(int)
            {
                sentinel tmp{*this};
                ++(*this);
                return tmp;
            }

            [[nodiscard]] bool constexpr operator==(iterator const &it) const { return false; }
        };

        using const_iterator = iterator;
        using const_sentinel = sentinel;

        [[nodiscard]] const_iterator cbegin() { return {this}; }
        [[nodiscard]] const_sentinel cend() { return {}; }

        [[nodiscard]] iterator begin() { return {this}; }
        [[nodiscard]] sentinel end() { return {}; }

    private:
        static std::array<Integer, 2> const constexpr INITIAL_PRIMES{2, 3};
        /* Sorted list of primes found so far.
         * Must end with an odd prime due to the search skipping every other number.
         */
        std::vector<Integer> discovered{INITIAL_PRIMES.cbegin(), INITIAL_PRIMES.cend()};

        static_assert(INITIAL_PRIMES.size() > 1 && INITIAL_PRIMES.back() % 2 == 1);

        /**
         * @brief
         *
         * @param discovered
         * all the primes that are below x
         * @param x
         * the number that will be tested
         * @return true if x is prime
         * @return false if x is not prime
         */
        static bool has_prime_divisors(std::vector<Integer> const &discovered, Integer x)
        {
            for (auto const &i : discovered)
            {
                if (i * i > x)
                {
                    return false;
                }
                if (x % i == 0)
                {
                    return true;
                }
            }
            return false;
        }

        /**
         * @brief Performs addition and checks if the result would wrap around
         *
         * @param x number to be added
         * @param y number to be added
         * @return the sum of the two numbers
         * @throw std::runtime_error the two numbers would wrap around
         */
        [[nodiscard]] static auto constexpr add_throw(Integer x, Integer y)
        {
            if (x > std::numeric_limits<Integer>::max() - y)
            {
                throw std::runtime_error("overflow");
            }
            return x + y;
        }

        [[nodiscard]] static Integer prime_search(std::vector<Integer> const &discovered)
        {
            Integer prime_candidate = add_throw(discovered.back(), 2);
            while (true)
            {
                if (!has_prime_divisors(discovered, prime_candidate))
                {
                    return prime_candidate;
                }
                prime_candidate = add_throw(prime_candidate, 2);
            }
        }

    public:
        void discover_next() { discovered.push_back(prime_search(discovered)); }
        [[nodiscard]] auto constexpr size() const { return discovered.size(); }
        [[nodiscard]] auto constexpr at(size_t i) const { return discovered[i]; }
        auto constexpr operator[](size_t i) const { return discovered[i]; }
        auto next() { return *(++(*this)); }
        [[nodiscard]] auto constexpr &back() const { return discovered.back(); }
        auto &get(size_t i)
        {
            while (size() < i)
            {
                discover_next();
            }
            return back();
        }
    };

    using PrimesIterator = PrimeFinderBase<uintmax_t>;
    using FastPrimesIterator = PrimeFinderBase<unsigned>;

    static_assert(std::input_iterator<FastPrimesIterator::iterator>);
    static_assert(std::input_iterator<FastPrimesIterator::const_iterator>);
    static_assert(
        std::sentinel_for<FastPrimesIterator::const_sentinel, FastPrimesIterator::const_iterator>);
    static_assert(std::sentinel_for<FastPrimesIterator::sentinel, FastPrimesIterator::iterator>);
} // namespace msd::primes

#endif /* BB16AA93_3809_4DDD_9077_CD9E457D9E6C */
