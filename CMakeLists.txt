cmake_minimum_required(VERSION 3.2)

project(libj VERSION 0.1)

include(CTest)
include(FetchContent)

set(CMAKE_C_STANDARD 11)

FetchContent_Declare(
    libgb
    GIT_REPOSITORY https://github.com/nolanrus/libgb.git
    GIT_TAG        bcebe9ef9ec50ffdd52df61dc9e3892e7ef4f1c9
)
FetchContent_Declare(
    libis
    GIT_REPOSITORY https://github.com/nolanrus/libis.git
    GIT_TAG        b2705de696e4a32044fafc1e3740e16e436db5de
)
FetchContent_Declare(
    libsb
    GIT_REPOSITORY https://github.com/nolanrus/libsb.git
    GIT_TAG        f94138109f24eba852b8ea2d464ce7bed9b487e9
)
FetchContent_Declare(
    libutf
    GIT_REPOSITORY https://github.com/nolanrus/libutf.git
    GIT_TAG        07245f57a12b1a14da3b394af37ee2407e8c1668
)
FetchContent_MakeAvailable(libgb libis libsb libutf)

if (BUILD_TESTING)
    if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-fsanitize=fuzzer-no-link,address)
        add_link_options(-fsanitize=fuzzer-no-link,address)
    endif()
endif()

add_subdirectory(include)
add_subdirectory(src)

if (BUILD_TESTING)
    add_subdirectory(test-unit)
    add_subdirectory(test-json-test-suite)
    add_subdirectory(test-libfuzzer)
endif()
