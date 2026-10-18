#include "hexagon/attitude.hpp"

#include <charconv>
#include <fstream>
#include <iostream>
#include <ostream>
#include <print>
#include <string>
#include <string_view>

namespace {

void print_usage(const char* program)
{
    std::println(stderr, "usage: {} <log_file> [--alpha <0..1>] [--output <file>]", program);
    std::println(stderr,
                 "  --alpha   low-pass filter factor for the accelerometer (default 1 = off)");
    std::println(stderr, "  --output  write the results to <file> instead of stdout");
}

bool parse_alpha(std::string_view value, double& alpha)
{
    const auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), alpha);
    return ec == std::errc{} && ptr == value.data() + value.size() && alpha > 0.0 && alpha <= 1.0;
}

// Reads samples from `input` and writes `time_ms;roll_deg;pitch_deg` rows to `output`.
void write_attitudes(std::istream& input, std::ostream& output, hexagon::AccelLowPassFilter& filter)
{
    std::println(output, "time_ms;roll_deg;pitch_deg");

    std::string line;
    int line_number = 0;
    while (std::getline(input, line)) {
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
        std::println(output, "{};{:.2f};{:.2f}", sample->timestamp_ms, attitude.roll_deg,
                     attitude.pitch_deg);
    }
}

} // namespace

int main(int argc, char* argv[]) // NOLINT(bugprone-exception-escape)
{
    if (argc < 2 || argc % 2 != 0) {
        print_usage(argv[0]);
        return 2;
    }

    double alpha = 1.0;
    const char* output_path = nullptr;
    for (int i = 2; i < argc; i += 2) {
        const std::string_view flag = argv[i];
        if (flag == "--alpha" && parse_alpha(argv[i + 1], alpha)) {
            continue;
        }
        if (flag == "--output") {
            output_path = argv[i + 1];
            continue;
        }
        print_usage(argv[0]);
        return 2;
    }

    std::ifstream input{argv[1]};
    if (!input) {
        std::println(stderr, "error: cannot open '{}'", argv[1]);
        return 1;
    }

    std::ofstream output_file;
    if (output_path != nullptr) {
        output_file.open(output_path);
        if (!output_file) {
            std::println(stderr, "error: cannot create '{}'", output_path);
            return 1;
        }
    }
    std::ostream& output = output_path != nullptr ? output_file : std::cout;

    hexagon::AccelLowPassFilter filter{alpha};
    write_attitudes(input, output, filter);

    output.flush();
    if (!output) {
        std::println(stderr, "error: failed to write output");
        return 1;
    }
    return 0;
}
