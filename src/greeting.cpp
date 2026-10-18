#include "hexagon/greeting.hpp"

#include <format>

namespace hexagon {

std::string make_greeting(std::string_view name)
{
    return std::format("Hello, {}!", name.empty() ? "world" : name);
}

} // namespace hexagon
