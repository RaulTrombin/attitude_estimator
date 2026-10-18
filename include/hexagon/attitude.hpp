#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

namespace hexagon {

//  Accelerometer reading, axes are in milli-g.
struct AccelSample {
    std::int64_t timestamp_ms{};
    double x_mg{};
    double y_mg{};
    double z_mg{};
};

// Attitude angles in degrees.
//   roll  in (-180, 180]
//   pitch in [-90, 90]
struct Attitude {
    double roll_deg{};
    double pitch_deg{};
};

// Parses one log line in the form "<time_ms>; <x>; <y>; <z>".
[[nodiscard]] std::optional<AccelSample> parse_sample(std::string_view line);

// Estimates roll and pitch from a static accelerometer reading using the
// aerospace rotation sequence (yaw, then pitch, then roll: R = Rx(roll) Ry(pitch) Rz(yaw)).
//
// The body frame follows the aerospace convention (x forward, y right, z down),
// so a level, motionless sensor reads (0, 0, -1 g). Solving
//     a = Rx(roll) Ry(pitch) Rz(yaw) * (0, 0, -1 g)
// gives
//     ax =  g sin(pitch)
//     ay = -g cos(pitch) sin(roll)
//     az = -g cos(pitch) cos(roll)
// and therefore
//     roll  = atan2(-ay, -az)
//     pitch = atan2( ax, sqrt(ay^2 + az^2))
[[nodiscard]] Attitude estimate_attitude(const AccelSample& sample);

// First-order low-pass filter applied to the accelerometer axes before the
// angles are computed. alpha = 1 means no filtering; smaller values smooth it.
class AccelLowPassFilter {
public:
    explicit AccelLowPassFilter(double alpha);

    [[nodiscard]] AccelSample update(const AccelSample& sample);

private:
    double alpha_;
    bool initialized_{false};
    AccelSample state_{};
};

} // namespace hexagon
