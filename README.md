# processo_hexagon

A modern C++23 project built with CMake and Ninja, with unit tests written in GoogleTest.

## Requirements

- CMake 3.28 or newer
- Ninja
- GCC 14 (`g++-14`) or Clang 18 (`clang++`)
- Optional: ccache, which speeds up rebuilds and is used automatically when installed

On Ubuntu 24.04:

```bash
sudo apt install cmake ninja-build g++-14 clang ccache
```

GoogleTest is downloaded automatically the first time you configure the project, so you don't need to install it.

## Build presets

All build settings are kept in `CMakePresets.json`. Pick one of these presets:

| Preset          | Compiler | Build type |
|-----------------|----------|------------|
| `gcc-debug`     | GCC 14   | Debug      |
| `gcc-release`   | GCC 14   | Release    |
| `clang-debug`   | Clang 18 | Debug      |
| `clang-release` | Clang 18 | Release    |

Each preset builds into its own folder, `build/<preset>/`.

To list them from the terminal:

```bash
cmake --list-presets
```

## Build

```bash
cmake --preset gcc-debug            # configure
cmake --build --preset gcc-debug    # compile
```

You only need to run the configure step the first time, or after you change `CMakeLists.txt` or `CMakePresets.json`. After a code change, running the build step is enough.

## Run

```bash
./build/gcc-debug/hexagon_app        # prints: Hello, world!
./build/gcc-debug/hexagon_app Raul   # prints: Hello, Raul!
```

## Run the tests

```bash
ctest --preset gcc-debug
```

To see GoogleTest's own output, run the test program directly:

```bash
./build/gcc-debug/tests/hexagon_tests
```

To run only the tests whose names match a pattern:

```bash
ctest --preset gcc-debug -R MakeGreeting
```

To build without the tests, configure with:

```bash
cmake --preset gcc-debug -DHEXAGON_BUILD_TESTS=OFF
```

## All in one

This builds the project and runs the tests:

```bash
cmake --preset gcc-debug && cmake --build --preset gcc-debug && ctest --preset gcc-debug
```

## VS Code

With the **CMake Tools** extension installed:

1. Open the project folder in VS Code.
2. Choose a configure preset from the status bar, or run `CMake: Select Configure Preset` from the command palette (`Ctrl+Shift+P`).
3. Use the status bar buttons to **Build** (`F7`), **Run** or **Debug**.
4. Tests appear in the **Testing** sidebar (the flask icon), where you can run or debug each test on its own.

## Project layout

```
.
├── CMakeLists.txt         # root build script
├── CMakePresets.json      # configure, build and test presets
├── include/hexagon/       # public headers of the core library
├── src/                   # core library implementation
├── app/                   # executable entry point (main.cpp)
└── tests/                 # GoogleTest unit tests
```
