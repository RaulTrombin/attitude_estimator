# Attitude estimator

A project to estimate current pitch and roll of a vehicle using a NXP's MMA8451Q, using aerospace rotation sequence and NXP's AN3461.

Reads an accelerometer log (`<time_ms>; <x_mg>; <y_mg>; <z_mg>` per line) and prints the roll and pitch
for each sample, computed with the aerospace rotation sequence (body frame x forward, y right, z down;
a level sensor reads `(0, 0, -1000)` mg):

```text
roll  = atan2(-ay, -az)
pitch = atan2( ax, sqrt(ay² + az²))
```

```bash
./build/gcc-debug/attitude_estimator example/attitude_exam.log              # time_ms;roll_deg;pitch_deg
./build/gcc-debug/attitude_estimator example/attitude_exam.log --alpha 0.1  # low-pass filtered
./build/gcc-debug/attitude_estimator example/attitude_exam.log --output attitude.csv  # write to a file
```


## Requirements

- CMake 3.28 or newer
- Ninja
- GCC 14 (`g++-14`) or Clang 18 (`clang++`)
- Optional: ccache, which speeds up rebuilds and is used automatically when installed

On Ubuntu 24.04:

```bash
sudo apt install cmake ninja-build g++-14 clang ccache
```

## Build

```bash
cmake --preset gcc-debug            # configure
cmake --build --preset gcc-debug    # compile
```

## Run the tests

```bash
ctest --preset gcc-debug
```

## Project layout

```
.
├── CMakeLists.txt         # root build script
├── CMakePresets.json      # configure, build and test presets
├── include/hexagon/       # public headers of the core library
├── src/                   # core library implementation
├── app/                   # executable entry point (attitude_estimator.cpp)
└── tests/                 # GoogleTest unit tests
```
