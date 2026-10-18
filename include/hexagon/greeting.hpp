#pragma once

#include <string>
#include <string_view>

namespace hexagon {

[[nodiscard]] std::string make_greeting(std::string_view name);

} // namespace hexagon
