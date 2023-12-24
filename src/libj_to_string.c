#include "libj_internal.h"

#include <libutf.h>

#include <assert.h>
#include <locale.h>

#define INDENT_PLACEHOLDER "$"

LibjToStringOptions libj_to_string_options_pretty = {
        .left_bracket_prefix = "",
        .left_bracket_postfix = "",
        .right_bracket_prefix = "\n" INDENT_PLACEHOLDER,
        .right_bracket_postfix = "",
        .left_brace_prefix = "",
        .left_brace_postfix = "",
        .right_brace_prefix = "\n" INDENT_PLACEHOLDER,
        .right_brace_postfix = "",
        .comma_in_object_prefix = "",
        .comma_in_object_postfix = "",
        .comma_in_array_prefix = "",
        .comma_in_array_postfix = "",
        .colon_prefix = "",
        .colon_postfix = " ",
        .member_prefix = "\n" INDENT_PLACEHOLDER,
        .member_postfix = "",
        .element_prefix = "\n" INDENT_PLACEHOLDER,
        .element_postfix = "",
        .indent_string = "    ",
        .ascii_only = false,
};

LibjToStringOptions libj_to_string_options_compact = {
        .left_bracket_prefix = "",
        .left_bracket_postfix = "",
        .right_bracket_prefix = "",
        .right_bracket_postfix = "",
        .left_brace_prefix = "",
        .left_brace_postfix = "",
        .right_brace_prefix = "",
        .right_brace_postfix = "",
        .comma_in_object_prefix = "",
        .comma_in_object_postfix = "",
        .comma_in_array_prefix = "",
        .comma_in_array_postfix = "",
        .colon_prefix = "",
        .colon_postfix = "",
        .member_prefix = "",
        .member_postfix = "",
        .element_prefix = "",
        .element_postfix = "",
        .indent_string = "",
        .ascii_only = false,
};

static LibjError append_fragment(Libj *libj, const char *format, ...) {
    LibjError err = LIBJ_ERROR_OK;
    LibsbBuilder *indent_builder = NULL;
    LibsbBuilder *fragment_builder = NULL;
    char *indent = NULL;
    size_t indent_size = 0;
    char *fragment = NULL;
    size_t fragment_size = 0;
    va_list args;
    va_start(args, format);
    if (!libj || !format) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = ESB(libsb_create(libj->libsb, &indent_builder));
    if (err) goto end;
    for (int i = 0; i < libj->depth; i++) {
        err = ESB(libsb_append(libj->libsb, indent_builder, "%s", libj->to_string_options->indent_string));
        if (err) goto end;
    }
    err = ESB(libsb_destroy_into(libj->libsb, &indent_builder, &indent, &indent_size));
    if (err) goto end;
    err = ESB(libsb_create(libj->libsb, &fragment_builder));
    if (err) goto end;
    err = ESB(libsb_append_v(libj->libsb, fragment_builder, format, args));
    if (err) goto end;
    err = ESB(libsb_replace(libj->libsb, fragment_builder, INDENT_PLACEHOLDER, indent));
    if (err) goto end;
    err = ESB(libsb_destroy_into(libj->libsb, &fragment_builder, &fragment, &fragment_size));
    if (err) goto end;
    err = ESB(libsb_append(libj->libsb, libj->builder, "%s", fragment));
    if (err) goto end;
end:
    if (libj) {
        ESB(libsb_destroy(libj->libsb, &indent_builder));
        ESB(libsb_destroy(libj->libsb, &fragment_builder));
    }
    free(fragment);
    free(indent);
    va_end(args);
    return err;
}

static LibjError append_string(Libj *libj, LibjString string) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = E(append_fragment(libj, "\""));
    if (err) goto end;
    for (char *p = string.value, *end = string.value + string.size; p != end; ++p) {
        char c = *p;
        if (libj->to_string_options->ascii_only) {
            uint16_t c16[2];
            uint32_t c32;
            LibutfC8Type type = libutf_c8_type(c);
            if (type < 0) {
                err = LIBJ_ERROR_SYNTAX;
                goto end;
            }
            int n = type;
            if (1 < n) {
                if (!libutf_c8_to_c32(p, &c32)) {
                    err = LIBJ_ERROR_SYNTAX;
                    goto end;
                }
                int length;
                if (!libutf_c32_to_c16(c32, &length, c16)) {
                    err = LIBJ_ERROR_SYNTAX;
                    goto end;
                }
                err = E(append_fragment(libj, "\\u%04x", c16[0]));
                if (err) goto end;
                if (c16[0]) {
                    err = E(append_fragment(libj, "\\u%04x", c16[1]));
                    if (err) goto end;
                }
                p += n - 1;
                continue;
            }
        }
        switch (c) {
        case '\"':
        case '\\':
            err = E(append_fragment(libj, "\\%c", c));
            break;
        case '\b':
            err = E(append_fragment(libj, "\\b"));
            break;
        case '\f':
            err = E(append_fragment(libj, "\\f"));
            break;
        case '\n':
            err = E(append_fragment(libj, "\\n"));
            break;
        case '\r':
            err = E(append_fragment(libj, "\\r"));
            break;
        case '\t':
            err = E(append_fragment(libj, "\\t"));
            break;
        default:
            if ((unsigned char) c < 0x20) {
                err = E(append_fragment(libj, "\\u%04x", (unsigned char) c));
            } else {
                err = E(append_fragment(libj, "%c", c));
            }
            break;
        }
        if (err) goto end;
    }
    err = E(append_fragment(libj, "\""));
    if (err) goto end;
end:
    return err;
}

static LibjError append_number(Libj *libj, const char *number) {
    LibjError err = LIBJ_ERROR_OK;
    LibsbBuilder *builder = NULL;
    char *good_number = NULL; // number with locale dependent decimal comma replaced with '.'
    size_t good_number_size = 0;
    locale_t previous_locale = uselocale(0);
    if (!libj || !number) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    uselocale(libj->c_locale);
    err = ESB(libsb_create(libj->libsb, &builder));
    if (err) goto end;
    err = ESB(libsb_append(libj->libsb, builder, "%s", number));
    if (err) goto end;
    struct lconv * lconv = localeconv();
    err = ESB(libsb_replace(libj->libsb, builder, lconv->decimal_point, "."));
    if (err) goto end;
    err = ESB(libsb_destroy_into(libj->libsb, &builder, &good_number, &good_number_size));
    if (err) goto end;
    err = E(append_fragment(libj, "%s", good_number));
    if (err) goto end;
end:
    uselocale(previous_locale);
    free(good_number);
    ESB(libsb_destroy(libj->libsb, &builder));
    return err;
}

static LibjError append_json(Libj *libj, LibjJson *json);

static LibjError append_object(Libj *libj, LibjJson *json) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    if (LIBJ_TYPE_OBJECT != json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    err = E(append_fragment(libj, "%s{%s",
                            libj->to_string_options->left_brace_prefix,
                            libj->to_string_options->left_brace_postfix));
    if (err) goto end;
    ++libj->depth;
    for (size_t i = 0; i < json->object.size; ++i) {
        if (i) {
            err = E(append_fragment(libj, "%s,%s",
                                    libj->to_string_options->comma_in_object_prefix,
                                    libj->to_string_options->comma_in_object_postfix));
            if (err) goto end;
        }
        err = E(append_fragment(libj, "%s", libj->to_string_options->member_prefix));
        if (err) goto end;
        LibjMember member = json->object.members[i];
        err = E(append_string(libj, member.name));
        if (err) goto end;
        err = E(append_fragment(libj, "%s:%s",
                                libj->to_string_options->colon_prefix,
                                libj->to_string_options->colon_postfix));
        if (err) goto end;
        err = E(append_json(libj, member.value));
        if (err) goto end;
        err = E(append_fragment(libj, "%s", libj->to_string_options->member_postfix));
        if (err) goto end;
    }
    --libj->depth;
    err = E(append_fragment(libj, "%s}%s",
                            libj->to_string_options->right_brace_prefix,
                            libj->to_string_options->right_brace_postfix));
    if (err) goto end;
end:
    return err;
}

static LibjError append_array(Libj *libj, LibjJson *json) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    if (LIBJ_TYPE_ARRAY != json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    err = E(append_fragment(libj, "%s[%s",
                            libj->to_string_options->left_bracket_prefix,
                            libj->to_string_options->left_bracket_postfix));
    if (err) goto end;
    ++libj->depth;
    for (size_t i = 0; i < json->array.size; ++i) {
        if (i) {
            err = E(append_fragment(libj, "%s,%s",
                                    libj->to_string_options->comma_in_array_prefix,
                                    libj->to_string_options->comma_in_array_postfix));
            if (err) goto end;
        }
        err = E(append_fragment(libj, "%s", libj->to_string_options->element_prefix));
        if (err) goto end;
        err = E(append_json(libj, json->array.elements[i]));
        if (err) goto end;
        err = E(append_fragment(libj, "%s", libj->to_string_options->element_postfix));
        if (err) goto end;
    }
    --libj->depth;
    err = E(append_fragment(libj, "%s]%s",
                            libj->to_string_options->right_bracket_prefix,
                            libj->to_string_options->right_bracket_postfix));
    if (err) goto end;
end:
    return err;
}

static LibjError append_json(Libj *libj, LibjJson *json) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    switch (json->type) {
    case LIBJ_TYPE_NULL:
        err = E(append_fragment(libj, "null"));
        break;
    case LIBJ_TYPE_STRING:
        err = E(append_string(libj, json->string));
        break;
    case LIBJ_TYPE_NUMBER:
        err = E(append_number(libj, json->string.value));
        break;
    case LIBJ_TYPE_BOOL:
        err = E(append_fragment(libj, "%s", json->boolean ? "true" : "false"));
        break;
    case LIBJ_TYPE_ARRAY:
        err = E(append_array(libj, json));
        break;
    case LIBJ_TYPE_OBJECT:
        err = E(append_object(libj, json));
        break;
    default:
        abort();
    }
    if (err) goto end;
end:
    return err;
}

LibjError libj_to_string(Libj *libj, LibjJson *json, char **json_string, LibjToStringOptions *options) {
    size_t json_string_size;
    return libj_to_string_ex(libj, json, json_string, &json_string_size, options);
}

LibjError libj_to_string_ex(Libj *libj, LibjJson *json, char **json_string, size_t *json_string_size,
                            LibjToStringOptions *options) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json || !json_string || !json_string_size || !options) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = ESB(libsb_create(libj->libsb, &libj->builder));
    if (err) goto end;
    libj->to_string_options = options;
    libj->depth = 0;
    err = E(append_json(libj, json));
    if (err) goto end;
    assert(!libj->depth);
    ESB(libsb_destroy_into(libj->libsb, &libj->builder, json_string, json_string_size));
    libj->builder = NULL;
end:
    if (libj) {
        libj->depth = 0;
        libj->to_string_options = NULL;
        ESB(libsb_destroy(libj->libsb, &libj->builder));
    }
    return err;
}
