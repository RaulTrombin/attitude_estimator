include(FetchContent)

FetchContent_Declare(googletest
    URL https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    FIND_PACKAGE_ARGS NAMES GTest)  # use an installed GoogleTest if one exists
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(hexagon_tests
    greeting_test.cpp)
target_link_libraries(hexagon_tests PRIVATE
    hexagon::core
    GTest::gtest_main)
set_target_properties(hexagon_tests PROPERTIES CXX_EXTENSIONS OFF)

include(GoogleTest)
gtest_discover_tests(hexagon_tests)
