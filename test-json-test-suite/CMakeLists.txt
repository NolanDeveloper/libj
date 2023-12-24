add_executable(libj_json_test_suite main.c)

target_link_libraries(
    libj_json_test_suite
    PUBLIC libj
)

FetchContent_Declare(
    JSONTestSuite
    GIT_REPOSITORY https://github.com/nst/JSONTestSuite.git
    GIT_TAG        984defc2deaa653cb73cd29f4144a720ec9efe7c
)
FetchContent_MakeAvailable(JSONTestSuite)

add_test(
    NAME json_test_suite 
    COMMAND libj_json_test_suite ${jsontestsuite_SOURCE_DIR}/test_parsing
)

