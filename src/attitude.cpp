#include "hexagon/attitude.hpp"

#include <array>
#include <charconv>
#include <cmath>
#include <numbers>
#include <stdexcept>

namespace hexagon {

namespace {

constexpr double rad_to_deg = 180.0 / std::numbers::pi;

std::string_view trim(std::string_view text)
{
    constexpr std::string_view whitespace = " \t\r\n";
    const auto first = text.find_first_not_of(whitespace);
    if (first == std::string_view::npos) {
        return {};
    }
    const auto last = text.find_last_not_of(whitespace);
    return text.substr(first, last - first + 1);
}

template <typename T>
bool parse_number(std::string_view text, T& value)
{
    text = trim(text);
    const auto* end = text.data() + text.size();
    const auto [ptr, ec] = std::from_chars(text.data(), end, value);
    return ec == std::errc{} && ptr == end;
}

}

std::optional<AccelSample> parse_sample(std::string_view line)
{
    std::array<std::string_view, 4> fields;
    std::size_t count = 0;
    while (true) {
        const auto sep = line.find(';');
        if (count == fields.size()) {
            return std::nullopt;
        }
        fields[count++] = line.substr(0, sep);
        if (sep == std::string_view::npos) {
            break;
        }
        line.remove_prefix(sep + 1);
    }
    if (count != fields.size()) {
        return std::nullopt;
    }

    AccelSample sample;
    if (!parse_number(fields[0], sample.timestamp_ms)) {
        return std::nullopt;
    }
    if (!parse_number(fields[1], sample.x_mg) || !parse_number(fields[2], sample.y_mg) ||
        !parse_number(fields[3], sample.z_mg)) {
        return std::nullopt;
    }
    return sample;
}

Attitude estimate_attitude(const AccelSample& sample)
{
    const double ax = sample.x_mg;
    const double ay = sample.y_mg;
    const double az = sample.z_mg;

    const double roll = std::atan2(-ay, -az);
    const double pitch = std::atan2(ax, std::hypot(ay, az));

    return {.roll_deg = roll * rad_to_deg, .pitch_deg = pitch * rad_to_deg};
}

AccelLowPassFilter::AccelLowPassFilter(double alpha) : alpha_{alpha}
{
    if (!(alpha > 0.0 && alpha <= 1.0)) {
        throw std::invalid_argument("alpha must be in (0, 1]");
    }
}

AccelSample AccelLowPassFilter::update(const AccelSample& sample)
{
    if (!initialized_) {
        state_ = sample;
        initialized_ = true;
        return state_;
    }
    state_.timestamp_ms = sample.timestamp_ms;
    state_.x_mg += alpha_ * (sample.x_mg - state_.x_mg);
    state_.y_mg += alpha_ * (sample.y_mg - state_.y_mg);
    state_.z_mg += alpha_ * (sample.z_mg - state_.z_mg);
    return state_;
}

}
