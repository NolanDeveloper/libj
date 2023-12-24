add_library(libj
        libj_essential.c
        libj_convenience.c
        libj_from_string.c
        libj_internal.h
        libj_to_string.c
        libj_utils.c
        libj_utils.h)
target_link_libraries(libj
        PUBLIC libj_interface
        PUBLIC libgb
        PUBLIC libis
        PUBLIC libsb
        PUBLIC libutf)
