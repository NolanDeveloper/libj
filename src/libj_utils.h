#ifndef LIBJ_UTILS_H
#define LIBJ_UTILS_H

#include "libj_internal.h"

LibjError libj_string_duplicate(Libj *libj, const char *src, size_t size, char **dst);

// Keep discarding characters from input as long is it's json whitespace characters.
// eof -- output parameter, whether end of file was reached
// c   -- output parameter, next character after whitespaces
LibjError libj_skip_whitespace(Libj *libj, LibisInputStream *input, bool *eof, char *c);

#endif

