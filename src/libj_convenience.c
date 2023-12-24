#include "libj_internal.h"
#include "libj_utils.h"

#include <libutf.h>

#include <stdio.h>
#include <string.h>

LibjError libj_object_add_string(Libj *libj, LibjJson *json, const char *name, const char *value) {
    LibjError err = LIBJ_ERROR_OK;
    LibjJson *json_value = NULL;
    if (!libj || !json || !name || !value) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    if (LIBJ_TYPE_OBJECT != json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    err = E(libj_string_create(libj, &json_value, value));
    if (err) goto end;
    err = E(libj_object_add(libj, json, name, json_value));
    if (err) goto end;
end:
    E(libj_free_json(libj, &json_value));
    return err;
}

LibjError libj_object_add_integer(Libj *libj, LibjJson *json, const char *name, int64_t value) {
    LibjError err = LIBJ_ERROR_OK;
    LibjJson *json_value = NULL;
    if (!libj || !json || !name) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    if (LIBJ_TYPE_OBJECT != json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    err = E(libj_integer_create(libj, &json_value, value));
    if (err) goto end;
    err = E(libj_object_add(libj, json, name, json_value));
    if (err) goto end;
end:
    E(libj_free_json(libj, &json_value));
    return err;
}

LibjError libj_object_add_real(Libj *libj, LibjJson *json, const char *name, double value) {
    LibjError err = LIBJ_ERROR_OK;
    LibjJson *json_value = NULL;
    if (!libj || !json || !name) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    if (LIBJ_TYPE_OBJECT != json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    err = E(libj_real_create(libj, &json_value, value));
    if (err) goto end;
    err = E(libj_object_add(libj, json, name, json_value));
    if (err) goto end;
end:
    E(libj_free_json(libj, &json_value));
    return err;
}

LibjError libj_object_add_number(Libj *libj, LibjJson *json, const char *name, const char *value) {
    LibjError err = LIBJ_ERROR_OK;
    LibjJson *json_value = NULL;
    if (!libj || !json || !name || !value) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    if (LIBJ_TYPE_OBJECT != json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    err = E(libj_number_create(libj, &json_value, value));
    if (err) goto end;
    err = E(libj_object_add(libj, json, name, json_value));
    if (err) goto end;
end:
    E(libj_free_json(libj, &json_value));
    return err;
}

LibjError libj_object_add_bool(Libj *libj, LibjJson *json, const char *name, bool value) {
    LibjError err = LIBJ_ERROR_OK;
    LibjJson *json_value = NULL;
    if (!libj || !json || !name) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    if (LIBJ_TYPE_OBJECT != json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    err = E(libj_bool_create(libj, &json_value, value));
    if (err) goto end;
    err = E(libj_object_add(libj, json, name, json_value));
    if (err) goto end;
end:
    E(libj_free_json(libj, &json_value));
    return err;
}

LibjError libj_object_add_null(Libj *libj, LibjJson *json, const char *name) {
    LibjError err = LIBJ_ERROR_OK;
    LibjJson *json_value = NULL;
    if (!libj || !json || !name) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    if (LIBJ_TYPE_OBJECT != json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    err = E(libj_null_create(libj, &json_value));
    if (err) goto end;
    err = E(libj_object_add(libj, json, name, json_value));
    if (err) goto end;
end:
    E(libj_free_json(libj, &json_value));
    return err;
}

LibjError libj_object_add(Libj *libj, LibjJson *json, const char *name, LibjJson *value) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json || !name || !value) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = E(libj_object_add_ex(libj, json, name, strlen(name), value));
    if (err) goto end;
end:
    return err;
}

LibjError libj_object_add_ex(Libj *libj, LibjJson *json, const char *name, size_t name_size, LibjJson *value) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json || !name || !value) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    if (LIBJ_TYPE_OBJECT != json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    err = E(libj_object_insert_at_ex(libj, json, json->object.size, name, name_size, value));
end:
    return err;
}

LibjError libj_object_count_versions(Libj *libj, LibjJson *json, const char *name, size_t *nversions) {
    if (!name) {
        return LIBJ_ERROR_BAD_ARGUMENT;
    }
    return libj_object_count_versions_ex(libj, json, name, strlen(name), nversions);
}

LibjError libj_object_get_version(Libj *libj, LibjJson *json, LibjJson **value, const char *name, size_t version) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json || !value || !name) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    *value = NULL;
    if (LIBJ_TYPE_OBJECT != json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    err = E(libj_object_get_version_ex(libj, json, value, name, strlen(name), version));
    if (err) goto end;
end:
    return err;
}

LibjError libj_object_get_string_(Libj *libj, LibjJson *json, char **value, const char *name, ...) {
    va_list args;
    va_start(args, name);
    LibjError err = libj_object_get_string_v(libj, json, value, name, args);
    va_end(args);
    return err;
}

LibjError libj_object_get_string_v(Libj *libj, LibjJson *json, char **value, const char *name, va_list args) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json || !value || !name) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    *value = NULL;
    if (LIBJ_TYPE_OBJECT != json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    LibjJson *json_value;
    err = E(libj_object_get_v(libj, json, &json_value, name, args));
    if (err) goto end;
    err = E(libj_get_string(libj, json_value, value));
    if (err) goto end;
end:
    return err;
}

LibjError
libj_object_get_string_or_default_(Libj *libj, LibjJson *json, char **value, char *default_value, const char *name,
                                   ...) {
    va_list args;
    va_start(args, name);
    LibjError err = libj_object_get_string_or_default_v(libj, json, value, default_value, name, args);
    va_end(args);
    return err;
}

LibjError libj_object_get_string_or_default_v(Libj *libj, LibjJson *json, char **value, char *default_value,
                                              const char *name, va_list args) {
    LibjError err = libj_object_get_string_v(libj, json, value, name, args);
    if (LIBJ_ERROR_NOT_FOUND == err) {
        *value = default_value;
        err = LIBJ_ERROR_OK;
    }
    return err;
}

LibjError libj_object_get_integer_(Libj *libj, LibjJson *json, int64_t *value, const char *name, ...) {
    va_list args;
    va_start(args, name);
    LibjError err = libj_object_get_integer_v(libj, json, value, name, args);
    va_end(args);
    return err;
}

LibjError libj_object_get_integer_v(Libj *libj, LibjJson *json, int64_t *value, const char *name, va_list args) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json || !value || !name) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    *value = 0;
    if (LIBJ_TYPE_OBJECT != json->type) {
        goto end;
    }
    LibjJson *json_value;
    err = E(libj_object_get_v(libj, json, &json_value, name, args));
    if (err) goto end;
    err = E(libj_get_integer(libj, json_value, value));
    if (err) goto end;
end:
    return err;
}

LibjError
libj_object_get_integer_or_default_(Libj *libj, LibjJson *json, int64_t *value, int64_t default_value,
                                    const char *name, ...) {
    va_list args;
    va_start(args, name);
    LibjError err = libj_object_get_integer_or_default_v(libj, json, value, default_value, name, args);
    va_end(args);
    return err;
}

LibjError
libj_object_get_integer_or_default_v(Libj *libj, LibjJson *json, int64_t *value, int64_t default_value,
                                     const char *name, va_list args) {
    LibjError err = libj_object_get_integer_v(libj, json, value, name, args);
    if (LIBJ_ERROR_NOT_FOUND == err) {
        *value = default_value;
        err = LIBJ_ERROR_OK;
    }
    return err;
}

LibjError libj_object_get_real_(Libj *libj, LibjJson *json, double *value, const char *name, ...) {
    va_list args;
    va_start(args, name);
    LibjError err = libj_object_get_real_v(libj, json, value, name, args);
    va_end(args);
    return err;
}

LibjError libj_object_get_real_v(Libj *libj, LibjJson *json, double *value, const char *name, va_list args) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json || !value || !name) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    *value = 0;
    if (LIBJ_TYPE_OBJECT != json->type) {
        goto end;
    }
    LibjJson *json_value;
    err = E(libj_object_get_v(libj, json, &json_value, name, args));
    if (err) goto end;
    err = E(libj_get_real(libj, json_value, value));
    if (err) goto end;
end:
    return err;
}

LibjError
libj_object_get_real_or_default_(Libj *libj, LibjJson *json, double *value, double default_value,
                                 const char *name, ...) {
    va_list args;
    va_start(args, name);
    LibjError err = libj_object_get_real_or_default_v(libj, json, value, default_value, name, args);
    va_end(args);
    return err;
}

LibjError
libj_object_get_real_or_default_v(Libj *libj, LibjJson *json, double *value, double default_value,
                                  const char *name, va_list args) {
    LibjError err = libj_object_get_real_v(libj, json, value, name, args);
    if (LIBJ_ERROR_NOT_FOUND == err) {
        *value = default_value;
        err = LIBJ_ERROR_OK;
    }
    return err;
}

LibjError libj_object_get_number_(Libj *libj, LibjJson *json, char **value, const char *name, ...) {
    va_list args;
    va_start(args, name);
    LibjError err = libj_object_get_number_v(libj, json, value, name, args);
    va_end(args);
    return err;
}

LibjError libj_object_get_number_v(Libj *libj, LibjJson *json, char **value, const char *name, va_list args) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json || !value || !name) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    *value = NULL;
    if (LIBJ_TYPE_OBJECT != json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    LibjJson *json_value;
    err = E(libj_object_get_v(libj, json, &json_value, name, args));
    if (err) goto end;
    err = E(libj_get_number(libj, json_value, value));
    if (err) goto end;
end:
    return err;
}

LibjError
libj_object_get_number_or_default_(Libj *libj, LibjJson *json, char **value, char *default_value, const char *name,
                                   ...) {
    va_list args;
    va_start(args, name);
    LibjError err = libj_object_get_number_or_default_v(libj, json, value, default_value, name, args);
    va_end(args);
    return err;
}

LibjError libj_object_get_number_or_default_v(Libj *libj, LibjJson *json, char **value, char *default_value,
                                              const char *name, va_list args) {
    LibjError err = libj_object_get_number_v(libj, json, value, name, args);
    if (LIBJ_ERROR_NOT_FOUND == err) {
        *value = default_value;
        err = LIBJ_ERROR_OK;
    }
    return err;
}

LibjError libj_object_get_bool_(Libj *libj, LibjJson *json, bool *value, const char *name, ...) {
    va_list args;
    va_start(args, name);
    LibjError err = libj_object_get_bool_v(libj, json, value, name, args);
    va_end(args);
    return err;
}

LibjError libj_object_get_bool_v(Libj *libj, LibjJson *json, bool *value, const char *name, va_list args) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json || !value || !name) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    *value = false;
    if (LIBJ_TYPE_OBJECT != json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    LibjJson *json_value;
    err = E(libj_object_get_v(libj, json, &json_value, name, args));
    if (err) goto end;
    err = E(libj_get_bool(libj, json_value, value));
    if (err) goto end;
end:
    return err;
}

LibjError
libj_object_get_bool_or_default_(Libj *libj, LibjJson *json, bool *value, bool default_value, const char *name, ...) {
    va_list args;
    va_start(args, name);
    LibjError err = libj_object_get_bool_or_default_v(libj, json, value, default_value, name, args);
    va_end(args);
    return err;
}

LibjError libj_object_get_bool_or_default_v(Libj *libj, LibjJson *json, bool *value, bool default_value,
                                            const char *name, va_list args) {
    LibjError err = libj_object_get_bool_v(libj, json, value, name, args);
    if (LIBJ_ERROR_NOT_FOUND == err) {
        *value = default_value;
        err = LIBJ_ERROR_OK;
    }
    return err;
}

LibjError libj_object_get_(Libj *libj, LibjJson *json, LibjJson **value, const char *name, ...) {
    va_list args;
    va_start(args, name);
    LibjError err = libj_object_get_v(libj, json, value, name, args);
    va_end(args);
    return err;
}

LibjError libj_object_get_v(Libj *libj, LibjJson *json, LibjJson **value, const char *name, va_list args) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json || !value || !name) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    *value = NULL;
    if (LIBJ_TYPE_OBJECT != json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    while (name) {
        err = E(libj_object_get_ex(libj, json, &json, name, strlen(name)));
        if (err) goto end;
        name = va_arg(args, char *);
    }
    *value = json;
end:
    return err;
}

LibjError libj_object_get_ex(Libj *libj, LibjJson *json, LibjJson **value, const char *name, size_t name_size) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json || !value || !name) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    *value = NULL;
    if (LIBJ_TYPE_OBJECT != json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    size_t number_of_versions;
    err = E(libj_object_count_versions_ex(libj, json, name, name_size, &number_of_versions));
    if (err) goto end;
    if (!number_of_versions) {
        err = LIBJ_ERROR_NOT_FOUND;
        goto end;
    }
    err = E(libj_object_get_version_ex(libj, json, value, name, name_size, number_of_versions - 1));
    if (err) goto end;
end:
    return err;
}

LibjError libj_object_set_null(Libj *libj, LibjJson *json, const char *name) {
    LibjError err = LIBJ_ERROR_OK;
    LibjJson *json_value = NULL;
    if (!libj || !json || !name) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    if (LIBJ_TYPE_OBJECT == json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    err = E(libj_null_create(libj, &json_value));
    if (err) goto end;
    err = E(libj_object_set(libj, json, name, json_value));
    if (err) goto end;
end:
    E(libj_free_json(libj, &json_value));
    return err;
}

LibjError libj_object_set_bool(Libj *libj, LibjJson *json, const char *name, bool value) {
    LibjError err = LIBJ_ERROR_OK;
    LibjJson *json_value = NULL;
    if (!libj || !json || !name) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    if (LIBJ_TYPE_OBJECT == json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    err = E(libj_bool_create(libj, &json_value, value));
    if (err) goto end;
    err = E(libj_object_set(libj, json, name, json_value));
    if (err) goto end;
end:
    E(libj_free_json(libj, &json_value));
    return err;
}

LibjError libj_object_set_integer(Libj *libj, LibjJson *json, const char *name, int64_t value) {
    LibjError err = LIBJ_ERROR_OK;
    LibjJson *json_value = NULL;
    if (!libj || !json || !name) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    if (LIBJ_TYPE_OBJECT == json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    err = E(libj_integer_create(libj, &json_value, value));
    if (err) goto end;
    err = E(libj_object_set(libj, json, name, json_value));
    if (err) goto end;
end:
    E(libj_free_json(libj, &json_value));
    return err;
}

LibjError libj_object_set_real(Libj *libj, LibjJson *json, const char *name, double value) {
    LibjError err = LIBJ_ERROR_OK;
    LibjJson *json_value = NULL;
    if (!libj || !json || !name) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    if (LIBJ_TYPE_OBJECT == json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    err = E(libj_real_create(libj, &json_value, value));
    if (err) goto end;
    err = E(libj_object_set(libj, json, name, json_value));
    if (err) goto end;
end:
    E(libj_free_json(libj, &json_value));
    return err;
}

LibjError libj_object_set_string(Libj *libj, LibjJson *json, const char *name, const char *value) {
    LibjError err = LIBJ_ERROR_OK;
    LibjJson *json_value = NULL;
    if (!libj || !json || !name) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    if (LIBJ_TYPE_OBJECT != json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    err = E(libj_string_create(libj, &json_value, value));
    if (err) goto end;
    err = E(libj_object_set(libj, json, name, json_value));
    if (err) goto end;
end:
    E(libj_free_json(libj, &json_value));
    return err;
}

LibjError libj_object_set(Libj *libj, LibjJson *json, const char *name, LibjJson *value) {
    return libj_object_set_ex(libj, json, name, strlen(name), value);
}

LibjError libj_object_set_ex(Libj *libj, LibjJson *json, const char *name, size_t name_size, LibjJson *value) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json || !name || !value) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    if (LIBJ_TYPE_OBJECT != json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    err = E(libj_object_remove_all_ex(libj, json, name, name_size));
    if (err) goto end;
    err = E(libj_object_add_ex(libj, json, name, name_size, value));
    if (err) goto end;
end:
    return err;
}

LibjError libj_object_get_member_at(Libj *libj, LibjJson *json, size_t i, const char **name, LibjJson **value) {
    size_t name_size;
    return libj_object_get_member_at_ex(libj, json, i, name, &name_size, value);
}

LibjError libj_object_remove_(Libj *libj, LibjJson *json, const char *name, ...) {
    va_list args;
    va_start(args, name);
    LibjError err = libj_object_remove_v(libj, json, name, args);
    va_end(args);
    return err;
}

LibjError libj_object_remove_v(Libj *libj, LibjJson *json, const char *name, va_list args) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json || !name) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    if (LIBJ_TYPE_OBJECT != json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    char *next_name = va_arg(args, char *);
    while (next_name) {
        err = E(libj_object_get_(libj, json, &json, name));
        if (err) goto end;
        next_name = va_arg(args, char *);
    }
    size_t number_of_versions;
    err = E(libj_object_count_versions(libj, json, name, &number_of_versions));
    if (err) goto end;
    err = E(libj_object_remove_ex(libj, json, name, strlen(name), number_of_versions - 1));
    if (err) goto end;
end:
    return err;
}

LibjError libj_object_remove_ex(Libj *libj, LibjJson *json, const char *name, size_t name_size, size_t version) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json || !name) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    if (LIBJ_TYPE_OBJECT != json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    size_t index;
    err = E(object_get_version_index_ex(json, name, name_size, version, &index));
    if (err) goto end;
    err = E(libj_object_remove_at(libj, json, index));
    if (err) goto end;
end:
    return err;
}

LibjError libj_object_remove_all_ex(Libj *libj, LibjJson *json, const char *name, size_t name_size) {
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json || !name) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    if (LIBJ_TYPE_OBJECT != json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    for (size_t i = 0; i < json->object.size; ++i) {
        const char *member_name = json->object.members[i].name.value;
        size_t member_name_size = json->object.members[i].name.size;
        if (member_name_size == name_size && !memcmp(member_name, name, name_size)) {
            err = E(libj_object_remove_at(libj, json, i));
            if (err) goto end;
        }
    }
end:
    return err;
}

static LibjError array_add(LibjJson *json, LibjJson **element) {
    LibjError err = LIBJ_ERROR_OK;
    if (!json || !element || !*element) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    if (LIBJ_TYPE_ARRAY != json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    LibjJson **new_elements = realloc(json->array.elements, (json->array.size + 1) * sizeof(LibjJson *));
    if (!new_elements) {
        err = LIBJ_ERROR_OUT_OF_MEMORY;
        goto end;
    }
    new_elements[json->array.size] = *element;
    json->array.elements = new_elements;
    ++json->array.size;
    *element = NULL;
end:
    return err;
}

LibjError libj_array_add_string(Libj *libj, LibjJson *json, const char *value) {
    LibjError err = LIBJ_ERROR_OK;
    LibjJson *json_value = NULL;
    if (!libj || !json) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = E(libj_string_create(libj, &json_value, value));
    if (err) goto end;
    err = E(array_add(json, &json_value));
    if (err) goto end;
end:
    E(libj_free_json(libj, &json_value));
    return err;
}

LibjError libj_array_add_integer(Libj *libj, LibjJson *json, int64_t value) {
    LibjError err = LIBJ_ERROR_OK;
    LibjJson *json_value = NULL;
    if (!libj || !json) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = E(libj_integer_create(libj, &json_value, value));
    if (err) goto end;
    err = E(array_add(json, &json_value));
    if (err) goto end;
end:
    E(libj_free_json(libj, &json_value));
    return err;
}

LibjError libj_array_add_real(Libj *libj, LibjJson *json, double value) {
    LibjError err = LIBJ_ERROR_OK;
    LibjJson *json_value = NULL;
    if (!libj || !json) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = E(libj_real_create(libj, &json_value, value));
    if (err) goto end;
    err = E(array_add(json, &json_value));
    if (err) goto end;
end:
    E(libj_free_json(libj, &json_value));
    return err;
}

LibjError libj_array_add_number(Libj *libj, LibjJson *json, const char *value) {
    LibjError err = LIBJ_ERROR_OK;
    LibjJson *json_value = NULL;
    if (!libj || !json) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = E(libj_number_create(libj, &json_value, value));
    if (err) goto end;
    err = E(array_add(json, &json_value));
    if (err) goto end;
end:
    E(libj_free_json(libj, &json_value));
    return err;
}

LibjError libj_array_add_bool(Libj *libj, LibjJson *json, bool value) {
    LibjError err = LIBJ_ERROR_OK;
    LibjJson *json_value = NULL;
    if (!libj || !json) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = E(libj_bool_create(libj, &json_value, value));
    if (err) goto end;
    err = E(array_add(json, &json_value));
    if (err) goto end;
end:
    E(libj_free_json(libj, &json_value));
    return err;
}

LibjError libj_array_add_null(Libj *libj, LibjJson *json) {
    LibjError err = LIBJ_ERROR_OK;
    LibjJson *json_value;
    if (!libj || !json) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    if (LIBJ_TYPE_ARRAY != json->type) {
        err = LIBJ_ERROR_BAD_TYPE;
        goto end;
    }
    err = E(libj_null_create(libj, &json_value));
    if (err) goto end;
    err = E(array_add(json, &json_value));
    if (err) goto end;
end:
    E(libj_free_json(libj, &json_value));
    return err;
}

LibjError libj_get_string(Libj *libj, LibjJson *json, char **value) {
    size_t value_size;
    LibjError err = LIBJ_ERROR_OK;
    if (!libj || !json || !value) {
        err = LIBJ_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = E(libj_get_string_ex(libj, json, value, &value_size));
    if (err) goto end;
    if (strlen(*value) != value_size) {
        err = LIBJ_ERROR_ZERO;
        goto end;
    }
end:
    return err;
}

LibjError libj_string_create(Libj *libj, LibjJson **json, const char *value) {
    return libj_string_create_ex(libj, json, value, strlen(value));
}

