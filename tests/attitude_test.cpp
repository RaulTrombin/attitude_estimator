#include "hexagon/attitude.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <numbers>

namespace {

constexpr double tolerance_deg = 1e-6;

// Builds the reading a motionless sensor would produce at the given attitude.
hexagon::AccelSample sample_at(double roll_deg, double pitch_deg)
{
    const double roll = roll_deg * std::numbers::pi / 180.0;
    const double pitch = pitch_deg * std::numbers::pi / 180.0;
    return {.timestamp_ms = 0,
            .x_mg = 1000.0 * std::sin(pitch),
            .y_mg = -1000.0 * std::cos(pitch) * std::sin(roll),
            .z_mg = -1000.0 * std::cos(pitch) * std::cos(roll)};
}

TEST(ParseSample, ReadsLogLine)
{
    const auto sample = hexagon::parse_sample("54741; 27; -22; -982\r");
    ASSERT_TRUE(sample.has_value());
    EXPECT_EQ(sample->timestamp_ms, 54741);
    EXPECT_DOUBLE_EQ(sample->x_mg, 27.0);
    EXPECT_DOUBLE_EQ(sample->y_mg, -22.0);
    EXPECT_DOUBLE_EQ(sample->z_mg, -982.0);
}

TEST(ParseSample, RejectsMalformedLines)
{
    EXPECT_FALSE(hexagon::parse_sample("54741; 27; -22").has_value());
    EXPECT_FALSE(hexagon::parse_sample("54741; 27; -22; -982; 5").has_value());
    EXPECT_FALSE(hexagon::parse_sample("54741; abc; -22; -982").has_value());
    EXPECT_FALSE(hexagon::parse_sample("").has_value());
}

TEST(EstimateAttitude, LevelIsZero)
{
    const auto attitude = hexagon::estimate_attitude({.z_mg = -1000.0});
    EXPECT_NEAR(attitude.roll_deg, 0.0, tolerance_deg);
    EXPECT_NEAR(attitude.pitch_deg, 0.0, tolerance_deg);
}

TEST(EstimateAttitude, RecoversKnownAngles)
{
    for (const double roll : {-170.0, -90.0, -30.0, 0.0, 15.0, 60.0, 135.0}) {
        for (const double pitch : {-80.0, -45.0, -10.0, 0.0, 20.0, 70.0}) {
            const auto attitude = hexagon::estimate_attitude(sample_at(roll, pitch));
            EXPECT_NEAR(attitude.roll_deg, roll, tolerance_deg)
                << "roll=" << roll << " pitch=" << pitch;
            EXPECT_NEAR(attitude.pitch_deg, pitch, tolerance_deg)
                << "roll=" << roll << " pitch=" << pitch;
        }
    }
}

TEST(EstimateAttitude, SignConventions)
{
    // Nose up: gravity appears along +x.
    EXPECT_NEAR(hexagon::estimate_attitude({.x_mg = 500.0, .z_mg = -866.0}).pitch_deg, 30.0, 0.01);
    // Right wing down: gravity appears along -y.
    EXPECT_NEAR(hexagon::estimate_attitude({.y_mg = -500.0, .z_mg = -866.0}).roll_deg, 30.0, 0.01);
}

TEST(EstimateAttitude, StaysFiniteAtVerticalPitch)
{
    const auto attitude = hexagon::estimate_attitude({.x_mg = 1000.0});
    EXPECT_TRUE(std::isfinite(attitude.roll_deg));
    EXPECT_NEAR(attitude.pitch_deg, 90.0, tolerance_deg);
}

TEST(AccelLowPassFilter, AlphaOneIsPassThrough)
{
    hexagon::AccelLowPassFilter filter{1.0};
    (void)filter.update({.x_mg = 0.0});
    EXPECT_DOUBLE_EQ(filter.update({.x_mg = 100.0}).x_mg, 100.0);
}

TEST(AccelLowPassFilter, SmoothsSteps)
{
    hexagon::AccelLowPassFilter filter{0.25};
    EXPECT_DOUBLE_EQ(filter.update({.x_mg = 0.0}).x_mg, 0.0);
    EXPECT_DOUBLE_EQ(filter.update({.x_mg = 100.0}).x_mg, 25.0);
    EXPECT_DOUBLE_EQ(filter.update({.x_mg = 100.0}).x_mg, 43.75);
}

TEST(AccelLowPassFilter, RejectsInvalidAlpha)
{
    EXPECT_THROW(hexagon::AccelLowPassFilter{0.0}, std::invalid_argument);
    EXPECT_THROW(hexagon::AccelLowPassFilter{1.5}, std::invalid_argument);
}

} // namespace
