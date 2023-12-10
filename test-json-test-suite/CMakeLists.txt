add_executable(libj_json_test_suite main.c)

target_link_libraries(libj_json_test_suite
        PUBLIC libj)

add_test(json_test_suite libj_json_test_suite)
