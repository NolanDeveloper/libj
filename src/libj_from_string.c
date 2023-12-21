#include "libj_internal.h"
#include "libj_utils.h"
#include <libutf.h>

#include <string.h>
#include <assert.h>
#include <locale.h>
#include <stdio.h>

#define LIBJ_MAX_DEPTH 100

static LibjError errorf(Libj *libj, const char *format, ...) {
    va_list args;
    va_start(args, format);
    LibsbBuilder *builder = NULL;
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !format) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = ESB(libsb_create(libj->libsb, &builder));
    if (err) goto end;
    err = ESB(libsb_append_v(libj->libsb, builder, format, args));
    if (err) goto end;
    size_t error_string_size;
    err = ESB(libsb_destroy_into(libj->libsb, &builder, &libj->error_string, &error_string_size));
    if (err) goto end;
end:
    if (libj) ESB(libsb_destroy(libj->libsb, &builder));
    va_end(args);
    return err;
}

static LibjError libj_skip_literal(Libj *libj, LibisInputStream *input, const char *literal) {
    LibjError err = LIBJ_ERROR_OK;
    char c;
    bool eof;
    if (!libj || !input || !literal) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = libj_skip_whitespace(libj, input, &eof, &c);
    if (err) goto end;
    for (size_t i = 0; i < strlen(literal); ++i) {
        if (c != literal[i]) {
            err = LIBJ_ERROR_SYNTAX;
            E(errorf(libj, "'%s' was expected", literal));
            goto end;
        }
        err = EIS(libis_skip_char(libj->libis, input, &eof, &c));
        if (err) goto end;
    }
end:
    return err;
}

LibjError libj_parse_value_object(Libj *libj, LibjJson **json, LibisInputStream *input) {
    LibjError err = LIBJ_ERROR_OK;
    LibjJson *result = NULL;
    LibjJson *name = NULL;
    LibjJson *value = NULL;
    char c;
    bool eof;
    if (!libj || !json || !input) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = E(libj_object_create(libj, &result));
    if (err) goto end;
    err = E(libj_skip_literal(libj, input, "{"));
    if (err) goto end;
    err = libj_skip_whitespace(libj, input, &eof, &c);
    if (err) goto end;
    if ('}' == c) {
        err = E(libj_skip_literal(libj, input, "}"));
        if (err) goto end;
        *json = result;
        result = NULL;
        goto end;
    }
    if (libj->depth == LIBJ_MAX_DEPTH) {
        err = LIBJ_ERROR_SYNTAX;
        E(errorf(libj, "too many nesting levels", 0));
        goto end;
    }
    ++libj->depth;
    for (;;) {
        err = E(libj_parse_value_string(libj, &name, input));
        assert((bool) err != (bool) name);
        if (err) goto end;
        err = E(libj_skip_literal(libj, input, ":"));
        if (err) goto end;
        err = E(libj_parse_value(libj, &value, input));
        if (err) goto end;
        err = E(libj_object_add_ex(libj, result, name->string.value, name->string.size, value));
        if (err) goto end;
        err = E(libj_free_json(libj, &name));
        if (err) goto end;
        err = E(libj_free_json(libj, &value));
        if (err) goto end;
        err = libj_skip_whitespace(libj, input, &eof, &c);
        if (err) goto end;
        if ('}' != c && ',' != c) {
            E(errorf(libj, "} or , was expected", 0));
            err = LIBJ_ERROR_SYNTAX;
            goto end;
        }
        if ('}' == c) {
            err = E(libj_skip_literal(libj, input, "}"));
            if (err) goto end;
            break;
        }
        err = EIS(libis_skip_char(libj->libis, input, &eof, &c));
        if (err) goto end;
    }
    --libj->depth;
    *json = result;
    result = NULL;
end:
    E(libj_free_json(libj, &result));
    E(libj_free_json(libj, &name));
    E(libj_free_json(libj, &value));
    return err;
}

LibjError libj_parse_value_array(Libj *libj, LibjJson **json, LibisInputStream *input) {
    LibjError err = LIBJ_ERROR_OK;
    LibjJson *result = NULL;
    LibjJson *element = NULL;
    char c;
    bool eof;
    if (!libj || !json || !input) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = E(libj_array_create(libj, &result));
    if (err) goto end;
    err = E(libj_skip_literal(libj, input, "["));
    if (err) goto end;
    err = libj_skip_whitespace(libj, input, &eof, &c);
    if (err) goto end;
    if (']' == c) {
        err = E(libj_skip_literal(libj, input, "]"));
        if (err) goto end;
        *json = result;
        result = NULL;
        goto end;
    }
    if (libj->depth == LIBJ_MAX_DEPTH) {
        err = LIBJ_ERROR_SYNTAX;
        E(errorf(libj, "too many nesting levels", 0));
        goto end;
    }
    ++libj->depth;
    for (;;) {
        err = E(libj_parse_value(libj, &element, input));
        if (err) goto end;
        err = E(libj_array_add(libj, result, element));
        if (err) goto end;
        err = E(libj_free_json(libj, &element));
        if (err) goto end;
        err = libj_skip_whitespace(libj, input, &eof, &c);
        if (err) goto end;
        if (']' != c && ',' != c) {
            E(errorf(libj, "] or , was expected", 0));
            err = LIBJ_ERROR_SYNTAX;
            goto end;
        }
        if (']' == c) {
            err = E(libj_skip_literal(libj, input, "]"));
            if (err) goto end;
            break;
        }
        err = EIS(libis_skip_char(libj->libis, input, &eof, &c));
        if (err) goto end;
    }
    --libj->depth;
    *json = result;
    result = NULL;
end:
    E(libj_free_json(libj, &result));
    E(libj_free_json(libj, &element));
    return err;
}

LibjError libj_parse_value_true(Libj *libj, LibjJson **json, LibisInputStream *input) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json || !input) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = E(libj_skip_literal(libj, input, "true"));
    if (err) goto end;
    err = E(libj_bool_create(libj, json, true));
    if (err) goto end;
end:
    return err;
}

LibjError libj_parse_value_false(Libj *libj, LibjJson **json, LibisInputStream *input) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json || !input) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = E(libj_skip_literal(libj, input, "false"));
    if (err) goto end;
    err = E(libj_bool_create(libj, json, false));
    if (err) goto end;
end:
    return err;
}

LibjError libj_parse_value_null(Libj *libj, LibjJson **json, LibisInputStream *input) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json || !input) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = E(libj_skip_literal(libj, input, "null"));
    if (err) goto end;
    err = E(libj_null_create(libj, json));
    if (err) goto end;
end:
    return err;
}

static LibjError consume_utf8_character(Libj *libj, LibisInputStream *input, LibgbBuffer *buffer) {
    LibjError err = LIBJ_ERROR_OK;
    char c;
    bool eof;
    if (!libj || !input || !buffer) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = EIS(libis_lookahead(libj->libis, input, &eof, 1, &c));
    if (err) goto end;
    LibutfC8Type type = libutf_c8_type(c);
    if (type < 0) {
        err = LIBJ_ERROR_SYNTAX;
        E(errorf(libj, "input is not UTF-8", 0));
        goto end;
    }
    int length = type;
    assert(0 < length && length < 5);
    char temp[5];
    int i;
    for (i = 0; i < length && c != EOF; ++i) {
        temp[i] = (char) c;
        err = EGB(libgb_append_buffer(libj->libgb, buffer, &temp[i], 1));
        if (err) goto end;
        err = EIS(libis_skip_char(libj->libis, input, &eof, &c));
        if (err) goto end;
    }
    if (i != length) {
        err = LIBJ_ERROR_SYNTAX;
        E(errorf(libj, "input is not UTF-8", 0));
        goto end;
    }
    temp[length] = '\0';
    uint32_t c32;
    if (!libutf_c8_to_c32(temp, &c32)) {
        err = LIBJ_ERROR_SYNTAX;
        E(errorf(libj, "input is not UTF-8", 0));
        goto end;
    }
end:
    return err;
}

static LibjError consume_hex(Libj *libj, LibisInputStream *input, int *value) {
    LibjError err = LIBJ_ERROR_OK;
    char c;
    bool eof;
    if (!value) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    *value = 0;
    err = EIS(libis_lookahead(libj->libis, input, &eof, 1, &c));
    if (err) goto end;
    if ('0' <= c && c <= '9') {
        *value = c - '0';
    } else if ('a' <= c && c <= 'f') {
        *value = 10 + c - 'a';
    } else if ('A' <= c && c <= 'F') {
        *value = 10 + c - 'A';
    } else {
        *value = -1;
        err = LIBJ_ERROR_SYNTAX;
        E(errorf(libj, "hexadecimal was expected", 0));
        goto end;
    }
    err = EIS(libis_skip_char(libj->libis, input, &eof, &c));
    if (err) goto end;
end:
    return err;
}

static LibjError consume_hex4(Libj *libj, LibisInputStream *input, uint32_t *u32) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !input || !u32) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    *u32 = 0;
    for (int i = 0; i < 4; ++i) {
        int digit;
        err = E(consume_hex(libj, input, &digit));
        if (err) goto end;
        *u32 = (*u32 * 16) + digit;
    }
end:
    return err;
}

static char escape(char c) {
    switch (c) {
    case '\\':
        return '\\';
    case '"':
        return '"';
    case '/':
        return '/';
    case 'b':
        return '\b';
    case 'f':
        return '\f';
    case 'n':
        return '\n';
    case 'r':
        return '\r';
    case 't':
        return '\t';
    }
    abort();
}

LibjError libj_consume_escape_sequence(Libj *libj, LibisInputStream *input, LibgbBuffer *buffer) {
    LibjError err = LIBJ_ERROR_OK;
    char c;
    bool eof;
    if (!libj || !input || !buffer) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = EIS(libis_skip_char(libj->libis, input, &eof, &c));
    if (err) goto end;
    switch (c) {
    case '\\':
    case '"':
    case '/':
    case 'b':
    case 'f':
    case 'n':
    case 'r':
    case 't':
        c = escape(c);
        err = EGB(libgb_append_buffer(libj->libgb, buffer, &c, 1));
        if (err) goto end;
        err = EIS(libis_skip_char(libj->libis, input, &eof, &c));
        if (err) goto end;
        break;
    case 'u': {
        err = EIS(libis_skip_char(libj->libis, input, &eof, &c));
        if (err) goto end;
        uint32_t p = 0;
        err = E(consume_hex4(libj, input, &p));
        if (err) goto end;
        LibutfC16Type type = libutf_c16_type((uint16_t) p);
        if (type == LIBUTF_UTF16_SURROGATE_LOW) {
            err = LIBJ_ERROR_SYNTAX;
            E(errorf(libj, "UTF-16 low surrogate comes first", 0));
            goto end;
        }
        if (type == LIBUTF_UTF16_SURROGATE_HIGH) {
            err = E(libj_skip_literal(libj, input, "\\u"));
            if (err) goto end;
            uint32_t next = 0;
            err = E(consume_hex4(libj, input, &next));
            if (err) goto end;
            LibutfC16Type next_type = libutf_c16_type((uint16_t) next);
            if (next_type != LIBUTF_UTF16_SURROGATE_LOW) {
                err = LIBJ_ERROR_SYNTAX;
                E(errorf(libj, "UTF-16 high surrogate is not followed by a low surrogate", 0));
                goto end;
            }
            uint16_t c16[2] = { p, next };
            if (!libutf_c16_to_c32(c16, &p)) {
                err = LIBJ_ERROR_SYNTAX;
                E(errorf(libj, "invalid UTF-16 surrogate pair", 0));
                goto end;
            }
        }
        char c8_bytes[5];
        int c8_size;
        if (!libutf_c32_to_c8(p, &c8_size, c8_bytes)) {
            err = LIBJ_ERROR_SYNTAX;
            E(errorf(libj, "input is not UTF-8", 0));
            goto end;
        }
        err = EGB(libgb_append_buffer(libj->libgb, buffer, c8_bytes, c8_size));
        if (err) goto end;
        break;
    }
    default:
        E(errorf(libj, "unknown escape sequence", 0));
        err = LIBJ_ERROR_SYNTAX;
        goto end;
    }
end:
    return err;
}

LibjError libj_parse_value_string(Libj *libj, LibjJson **json, LibisInputStream *input) {
    LibjError err = LIBJ_ERROR_OK;
    LibgbBuffer *buffer = NULL;
    char *string_value = NULL;
    size_t string_size = 0;
    char null = '\0';
    char c;
    bool eof;
    if (!libj || !json || !input) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    *json = NULL;
    err = E(libj_skip_literal(libj, input, "\""));
    if (err) goto end;
    err = EGB(libgb_create(libj->libgb, &buffer));
    if (err) goto end;
    for (;;) {
        err = EIS(libis_lookahead(libj->libis, input, &eof, 1, &c));
        if (err) goto end;
        if (EOF == c) {
            err = LIBJ_ERROR_SYNTAX;
            E(errorf(libj, "unexpected end of file", 0));
            goto end;
        }
        switch (c) {
        case '\"':
            err = E(libj_skip_literal(libj, input, "\""));
            if (err) goto end;
            err = EGB(libgb_append_buffer(libj->libgb, buffer, &null, 1));
            if (err) goto end;
            err = EGB(libgb_destroy_into(libj->libgb, &buffer, &string_value, &string_size));
            if (err) goto end;
            --string_size;
            err = E(libj_string_create_ex(libj, json, string_value, string_size));
            if (err) goto end;
            goto end;
        case '\x00':
            E(errorf(libj, "null character is not escaped", 0));
            err = LIBJ_ERROR_SYNTAX;
            goto end;
        case '\\':
            err = E(libj_consume_escape_sequence(libj, input, buffer));
            if (err) goto end;
            break;
        default:
            if ((unsigned char) c < 0x20) {
                E(errorf(libj, "control character is not escaped", 0));
                err = LIBJ_ERROR_SYNTAX;
                goto end;
            }
            err = E(consume_utf8_character(libj, input, buffer));
            if (err) goto end;
            break;
        }
    }
end:
    free(string_value);
    if (libj) EGB(libgb_destroy(libj->libgb, &buffer));
    return err;
}

static LibjError consume_sign(Libj *libj, LibisInputStream *input, LibgbBuffer *buffer) {
    LibjError err = LIBJ_ERROR_OK;
    char c;
    bool eof;
    if (!libj || !input || !buffer) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = EIS(libis_lookahead(libj->libis, input, &eof, 1, &c));
    if (err) goto end;
    if ('+' == c || '-' == c) {
        char t = c;
        err = EGB(libgb_append_buffer(libj->libgb, buffer, &t, 1));
        if (err) goto end;
        err = EIS(libis_skip_char(libj->libis, input, &eof, &c));
        if (err) goto end;
    }
end:
    return err;
}

static LibjError consume_digit(Libj *libj, LibisInputStream *input, LibgbBuffer *buffer) {
    LibjError err = LIBJ_ERROR_OK;
    char c;
    bool eof;
    if (!libj || !input || !buffer) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = EIS(libis_lookahead(libj->libis, input, &eof, 1, &c));
    if (err) goto end;
    if ('0' <= c && c <= '9') {
        char t = c;
        err = EGB(libgb_append_buffer(libj->libgb, buffer, &t, 1));
        if (err) goto end;
        err = EIS(libis_skip_char(libj->libis, input, &eof, &c));
        if (err) goto end;
    } else {
        E(errorf(libj, "a digit was expected", 0));
        err = LIBJ_ERROR_SYNTAX;
        goto end;
    }
end:
    return err;
}

static LibjError consume_digits(Libj *libj, LibisInputStream *input, LibgbBuffer *buffer) {
    LibjError err = LIBJ_ERROR_OK;
    char c;
    bool eof;
    if (!libj || !input || !buffer) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = EIS(libis_lookahead(libj->libis, input, &eof, 1, &c));
    if (err) goto end;
    while ('0' <= c && c <= '9') {
        char t = c;
        err = EGB(libgb_append_buffer(libj->libgb, buffer, &t, 1));
        if (err) goto end;
        err = EIS(libis_skip_char(libj->libis, input, &eof, &c));
        if (err) goto end;
    }
end:
    return err;
}

static LibjError consume_integer(Libj *libj, LibisInputStream *input, LibgbBuffer *buffer) {
    LibjError err = LIBJ_ERROR_OK;
    char c;
    bool eof;
    if (!libj || !input || !buffer) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = EIS(libis_lookahead(libj->libis, input, &eof, 1, &c));
    if (err) goto end;
    if ('-' == c) {
        err = EGB(libgb_append_buffer(libj->libgb, buffer, &c, 1));
        if (err) goto end;
        err = EIS(libis_skip_char(libj->libis, input, &eof, &c));
        if (err) goto end;
    }
    if ('0' == c) {
        err = EGB(libgb_append_buffer(libj->libgb, buffer, &c, 1));
        if (err) goto end;
        err = EIS(libis_skip_char(libj->libis, input, &eof, &c));
        if (err) goto end;
        goto end;
    } else {
        err = E(consume_digit(libj, input, buffer));
        if (err) goto end;
        err = E(consume_digits(libj, input, buffer));
        if (err) goto end;
    }
end:
    return err;
}

static LibjError consume_fractional(Libj *libj, LibisInputStream *input, LibgbBuffer *buffer) {
    LibjError err = LIBJ_ERROR_OK;
    char c;
    bool eof;
    if (!libj || !input || !buffer) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = EIS(libis_lookahead(libj->libis, input, &eof, 1, &c));
    if (err) goto end;
    if ('.' == c) {
        struct lconv *lconv = localeconv();
        err = EGB(libgb_append_buffer(libj->libgb, buffer, lconv->decimal_point, strlen(lconv->decimal_point)));
        if (err) goto end;
        err = EIS(libis_skip_char(libj->libis, input, &eof, &c));
        if (err) goto end;
        err = E(consume_digit(libj, input, buffer));
        if (err) goto end;
        err = E(consume_digits(libj, input, buffer));
        if (err) goto end;
    }
end:
    return err;
}

static LibjError consume_exponent(Libj *libj, LibisInputStream *input, LibgbBuffer *buffer) {
    LibjError err = LIBJ_ERROR_OK;
    char c;
    bool eof;
    if (!libj || !input || !buffer) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = EIS(libis_lookahead(libj->libis, input, &eof, 1, &c));
    if (err) goto end;
    if ('e' == c || 'E' == c) {
        char t = c;
        err = EGB(libgb_append_buffer(libj->libgb, buffer, &t, 1));
        if (err) goto end;
        err = EIS(libis_skip_char(libj->libis, input, &eof, &c));
        if (err) goto end;
        err = E(consume_sign(libj, input, buffer));
        if (err) goto end;
        err = E(consume_digit(libj, input, buffer));
        if (err) goto end;
        err = E(consume_digits(libj, input, buffer));
        if (err) goto end;
    }
end:
    return err;
}

LibjError libj_parse_value_number(Libj *libj, LibjJson **json, LibisInputStream *input) {
    LibjError err = LIBJ_ERROR_OK;
    LibgbBuffer *buffer = NULL;
    if (!libj || !json || !input) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = EGB(libgb_create(libj->libgb, &buffer));
    if (err) goto end;
    err = E(consume_integer(libj, input, buffer));
    if (err) goto end;
    err = E(consume_fractional(libj, input, buffer));
    if (err) goto end;
    err = E(consume_exponent(libj, input, buffer));
    if (err) goto end;
    char null = '\0';
    err = EGB(libgb_append_buffer(libj->libgb, buffer, &null, 1));
    if (err) goto end;
    *json = malloc(sizeof(LibjJson));
    if (!*json) {
        err = LIBJ_ERROR_OUT_OF_MEMORY;
        goto end;
    }
    (*json)->type = LIBJ_TYPE_NUMBER;
    err = EGB(libgb_destroy_into(libj->libgb, &buffer, &(*json)->string.value, &(*json)->string.size));
    if (err) goto end;
end:
    EGB(libgb_destroy(libj->libgb, &buffer));
    return err;
}

LibjError libj_parse_value(Libj *libj, LibjJson **json, LibisInputStream *input) {
    LibjError err = LIBJ_ERROR_OK;
    char c;
    bool eof;
    if (!libj || !json || !input) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = libj_skip_whitespace(libj, input, &eof, &c);
    if (err) goto end;
    switch (c) {
    case '{':
        err = E(libj_parse_value_object(libj, json, input));
        break;
    case '[':
        err = E(libj_parse_value_array(libj, json, input));
        break;
    case 't':
        err = E(libj_parse_value_true(libj, json, input));
        break;
    case 'f':
        err = E(libj_parse_value_false(libj, json, input));
        break;
    case 'n':
        err = E(libj_parse_value_null(libj, json, input));
        break;
    case '"':
        err = E(libj_parse_value_string(libj, json, input));
        break;
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        err = E(libj_parse_value_number(libj, json, input));
        break;
    default:
        E(errorf(libj, "json value was expected", 0));
        err = LIBJ_ERROR_SYNTAX;
        break;
    }
end:
    return err;
}

LibjError libj_from_string(Libj *libj, LibjJson **json, const char *input_string, const char **error_string) {
    LibjError err = LIBJ_ERROR_OK;
    LibisSource *source = NULL;
    LibisInputStream *input = NULL;
    if (!libj || !json || !input_string) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = EIS(libis_source_create_from_buffer(libj->libis, &source, input_string, strlen(input_string), false));
    if (err) goto end;
    err = EIS(libis_create(libj->libis, &input, &source, 1));
    if (err) goto end;
    err = E(libj_from_input_stream(libj, json, input, error_string));
    if (err) goto end;
end:
    if (libj) EIS(libis_destroy(libj->libis, &input));
    return err;
}

LibjError libj_skip_bom(Libj *libj, LibisInputStream *input) {
    LibjError err = LIBJ_ERROR_OK;
    static const char *bom = "\xEF\xBB\xBF";
    char c;
    bool eof;
    if (!libj || !input) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = EIS(libis_lookahead(libj->libis, input, &eof, 1, &c));
    if (err) goto end;
    if (c != bom[0]) {
        goto end;
    }
    err = EIS(libis_skip_char(libj->libis, input, &eof, &c));
    if (err) goto end;
    if (c == EOF) {
        err = LIBJ_ERROR_SYNTAX;
        E(errorf(libj, "unexpected end of file", 0));
        goto end;
    }
    if (c != bom[1]) {
        err = LIBJ_ERROR_SYNTAX;
        E(errorf(libj, "unexpected byte '%c'(0x%02X)", c));
        goto end;
    }
    err = EIS(libis_skip_char(libj->libis, input, &eof, &c));
    if (err) goto end;
    if (c == EOF) {
        err = LIBJ_ERROR_SYNTAX;
        E(errorf(libj, "unexpected end of file", 0));
        goto end;
    }
    if (c != bom[2]) {
        err = LIBJ_ERROR_SYNTAX;
        E(errorf(libj, "unexpected byte '%c'(0x%02X)", c));
        goto end;
    }
end:
    return err;
}

LibjError libj_from_input_stream(Libj *libj, LibjJson **json, LibisInputStream *input, const char **error_string) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json || !input || !error_string) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    free(libj->error_string);
    libj->error_string = NULL;
    err = E(libj_skip_bom(libj, input));
    if (err) {
        *error_string = libj->error_string;
        goto end;
    }
    libj->depth = 0;
    err = E(libj_parse_value(libj, json, input));
    if (err) {
        *error_string = libj->error_string;
        goto end;
    }
    *error_string = "";
    assert(!libj->depth);
end:
    return err;
}
