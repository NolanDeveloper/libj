#include <libj.h>

#define E(libj_call) do { \
        LibjError err = (libj_call); \
        if (err) { \
            const char *err_str = libj_error_to_string(err); \
            printf(__FILE__":%d at %s: libj error '"#libj_call"' returned %s\n", __LINE__, __func__, err_str); \
            exit(EXIT_FAILURE); \
        } \
    } while (0);


int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    Libj *libj = NULL;
    LibjJson *json = NULL;
    const char *error_string;
    int result = 0;
    E(libj_start(&libj));
    LibjError err = libj_from_string_ex(libj, &json, (const char *)data, size, &error_string);
    if (err != LIBJ_ERROR_OK) {
        result = -1;
    }
    E(libj_free_json(libj, &json));
    E(libj_finish(&libj));
    return result;
}

