add_executable(libj_tests
        main.c
        sanity.c
        test.h)

target_link_libraries(libj_tests
        PUBLIC libj)

add_test(unit_test libj_tests)
