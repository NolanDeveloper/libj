#include "libj_utils.h"

#include <string.h>

LibjError libj_string_duplicate(Libj *libj, const char *src, size_t size, char **dst) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !src || !dst) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    *dst = malloc(size + 1);
    if (!*dst) {
        err = LIBJ_ERROR_OUT_OF_MEMORY;
        goto end;
    }
    memcpy(*dst, src, size);
    (*dst)[size] = '\0';
end:
    return err;
}

static bool is_space(char c) {
    return c && strchr("\x20\x09\x0A\x0D", c);
}

LibjError libj_skip_whitespace(Libj *libj, LibisInputStream *input, bool *eof, char *c) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !input || !eof || !c) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = EIS(libis_lookahead(libj->libis, input, eof, 1, c));
    if (err) goto end;
    while (is_space(*c)) {
        err = EIS(libis_skip_char(libj->libis, input, eof, c));
        if (err) goto end;
    }
end:
    return err;
}

