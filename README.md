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
./build/debug/attitude_estimator example/attitude_exam.log              # time_ms;roll_deg;pitch_deg
./build/debug/attitude_estimator example/attitude_exam.log --alpha 0.1  # low-pass filtered
./build/debug/attitude_estimator example/attitude_exam.log --output attitude.csv  # write to a file
```


## Requirements

- CMake 3.28 or newer
- Ninja
- Clang 18 (`clang++`); on Linux

On Ubuntu 24.04:

```bash
sudo apt install cmake ninja-build g++-14 clang ccache
```

## Build

```bash
cmake --preset debug            # configure
cmake --build --preset debug    # compile
```

## Run the tests

```bash
ctest --preset debug
```

## Format and lint

```bash
git ls-files '*.cpp' '*.hpp' | xargs clang-format -i         # apply formatting
cmake --preset debug
run-clang-tidy -p build/debug "$PWD/(src|app|tests)/"         # lint
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
