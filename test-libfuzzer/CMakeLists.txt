if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
    add_executable(fuzzer main.c)
    target_link_libraries(fuzzer PUBLIC libj)
    target_link_options(fuzzer PUBLIC -fsanitize=fuzzer,address)
endif()

