#ifndef TEST_H
#define TEST_H

#include <libj.h>

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern Libj *libj;

#define E(libj_call) do { \
        LibjError err = (libj_call); \
        if (err) { \
            const char *err_str = libj_error_to_string(err); \
            printf(__FILE__":%d at %s: libj error '"#libj_call"' returned %s\n", __LINE__, __func__, err_str); \
            exit(EXIT_FAILURE); \
        } \
    } while (0);

void sanity_check(void);

#endif

