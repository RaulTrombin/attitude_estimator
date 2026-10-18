cmake_minimum_required(VERSION 3.28)

project(processo_hexagon
    VERSION 0.1.0
    DESCRIPTION "Modern C++ project"
    LANGUAGES CXX)

# Required by clang-tidy.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# Use the compiler cache if it is installed
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

# ---- Library: all the real logic lives here ----
add_library(hexagon_core
    src/attitude.cpp)
add_library(hexagon::core ALIAS hexagon_core)

target_include_directories(hexagon_core PUBLIC
    ${PROJECT_SOURCE_DIR}/include)

# Require C++23 for this target and anything that links to it.
target_compile_features(hexagon_core PUBLIC cxx_std_23)
set_target_properties(hexagon_core PROPERTIES CXX_EXTENSIONS OFF)

# ---- Executable: thin entry point ----
add_executable(attitude_estimator app/attitude_estimator.cpp)
target_link_libraries(attitude_estimator PRIVATE hexagon::core)
set_target_properties(attitude_estimator PROPERTIES CXX_EXTENSIONS OFF)

# ---- Tests ----
option(HEXAGON_BUILD_TESTS "Build the unit tests" ON)
if(HEXAGON_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
