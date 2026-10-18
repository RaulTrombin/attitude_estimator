#include "hexagon/greeting.hpp"

#include <gtest/gtest.h>

namespace {

TEST(MakeGreeting, UsesGivenName)
{
    EXPECT_EQ(hexagon::make_greeting("Raul"), "Hello, Raul!");
}

TEST(MakeGreeting, FallsBackToWorldWhenNameIsEmpty)
{
    EXPECT_EQ(hexagon::make_greeting(""), "Hello, world!");
}

} // namespace
