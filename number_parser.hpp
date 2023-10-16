#ifndef E0FC52B4_E39E_4C42_9179_EC1B0AAE7494
#define E0FC52B4_E39E_4C42_9179_EC1B0AAE7494

#include <optional>
#include <stdexcept>
#include <string>

template <typename Int> struct number_parser;
template <> struct number_parser<int>
{
    static auto const constexpr function = [](std::string const &str) { return std::stoi(str); };
};
template <> struct number_parser<long>
{
    static auto const constexpr function = [](std::string const &str) { return std::stol(str); };
};
template <> struct number_parser<long long>
{
    static auto const constexpr function = [](std::string const &str) { return std::stoll(str); };
};
template <> struct number_parser<unsigned long>
{
    static auto const constexpr function = [](std::string const &str) { return std::stoul(str); };
};
template <> struct number_parser<unsigned long long>
{
    static auto const constexpr function = [](std::string const &str) { return std::stoull(str); };
};
template <> struct number_parser<float>
{
    static auto const constexpr function = [](std::string const &str) { return std::stof(str); };
};
template <> struct number_parser<double>
{
    static auto const constexpr function = [](std::string const &str) { return std::stod(str); };
};
template <> struct number_parser<long double>
{
    static auto const constexpr function = [](std::string const &str) { return std::stold(str); };
};

template <typename Number>
inline auto const constexpr number_parser_f = number_parser<Number>::function;

template <typename Number> auto constexpr parse_num(std::string str)
{
    using opt_t = std::optional<Number>;

    std::string_view error_message;
    try
    {
        return opt_t{number_parser_f<Number>(str)};
    }
    catch (std::invalid_argument &)
    {
        error_message = "the given argument is not an integer";
    }
    catch (std::out_of_range &)
    {
        error_message = "the given argument is too big";
    }

    // TODO return error_message when std::expected is finally implemented
    (void)error_message;
    return opt_t{};
}

#endif /* E0FC52B4_E39E_4C42_9179_EC1B0AAE7494 */
