#include "hexagon/attitude.hpp"

#include <charconv>
#include <fstream>
#include <print>
#include <string>
#include <string_view>

namespace {

void print_usage(const char* program)
{
    std::println(stderr, "usage: {} <log_file> [--alpha <0..1>]", program);
    std::println(stderr, "  --alpha  low-pass filter factor for the accelerometer (default 1 = off)");
}

} // namespace

int main(int argc, char* argv[])
{
    if (argc != 2 && argc != 4) {
        print_usage(argv[0]);
        return 2;
    }

    double alpha = 1.0;
    if (argc == 4) {
        const std::string_view flag = argv[2];
        const std::string_view value = argv[3];
        const auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), alpha);
        if (flag != "--alpha" || ec != std::errc{} || ptr != value.data() + value.size() ||
            !(alpha > 0.0 && alpha <= 1.0)) {
            print_usage(argv[0]);
            return 2;
        }
    }

    std::ifstream file{argv[1]};
    if (!file) {
        std::println(stderr, "error: cannot open '{}'", argv[1]);
        return 1;
    }

    hexagon::AccelLowPassFilter filter{alpha};
    std::println("time_ms;roll_deg;pitch_deg");

    std::string line;
    int line_number = 0;
    while (std::getline(file, line)) {
        ++line_number;
        if (line.find_first_not_of(" \t\r") == std::string::npos) {
            continue;
        }
        const auto sample = hexagon::parse_sample(line);
        if (!sample) {
            std::println(stderr, "warning: line {} is malformed, skipped", line_number);
            continue;
        }
        const auto attitude = hexagon::estimate_attitude(filter.update(*sample));
        std::println("{};{:.2f};{:.2f}", sample->timestamp_ms, attitude.roll_deg,
                     attitude.pitch_deg);
    }
    return 0;
}
