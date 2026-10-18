#include "hexagon/greeting.hpp"

#include <print>

int main(int argc, char* argv[])
{
    const std::string_view name = argc > 1 ? argv[1] : "";
    std::println("{}", hexagon::make_greeting(name));
    return 0;
}
