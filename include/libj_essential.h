#ifndef LIBJ_ESSENTIAL_H
#define LIBJ_ESSENTIAL_H

#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <libis.h>

/* Type of json. */
typedef enum {
    LIBJ_TYPE_NULL,
    LIBJ_TYPE_STRING,
    LIBJ_TYPE_NUMBER,
    LIBJ_TYPE_BOOL,
    LIBJ_TYPE_ARRAY,
    LIBJ_TYPE_OBJECT,
} LibjType;

/* Error type. */
typedef enum {
    LIBJ_ERROR_OK,
    LIBJ_ERROR_OUT_OF_MEMORY,
    LIBJ_ERROR_BAD_TYPE,
    LIBJ_ERROR_BAD_ARGUMENT,
    LIBJ_ERROR_NOT_FOUND,
    LIBJ_ERROR_PRECISION,
    LIBJ_ERROR_SYNTAX,
    LIBJ_ERROR_IO,
    LIBJ_ERROR_ZERO,
} LibjError;

/* Options for libj_to_string. The string "$" will be replaced
 * with LibjToStringOptions::indent_string repeated as many times as current
 * nesting level. */
typedef struct {
    char *left_bracket_prefix;
    char *left_bracket_postfix;
    char *right_bracket_prefix;
    char *right_bracket_postfix;
    char *left_brace_prefix;
    char *left_brace_postfix;
    char *right_brace_prefix;
    char *right_brace_postfix;
    char *comma_in_object_prefix;
    char *comma_in_object_postfix;
    char *comma_in_array_prefix;
    char *comma_in_array_postfix;
    char *colon_prefix;
    char *colon_postfix;
    char *member_prefix;
    char *member_postfix;
    char *element_prefix;
    char *element_postfix;
    char *indent_string;
    bool ascii_only;
} LibjToStringOptions;

/* A type of json value. */
typedef struct LibjJson_ LibjJson;

/* A type of library context that must be passed into libj_* functions. */
typedef struct Libj_ Libj;

/* Create and initialize libj object that must be passed into most of the functions
 * of this library. */
LibjError libj_start(Libj **libj);

/* Release resources taken by libj object. *libj == NULL is allowed. */
LibjError libj_finish(Libj **libj);

/* Convert LibjType to const char *. Example: LIBJ_TYPE_NULL -> "LIBJ_TYPE_NULL". */
const char *libj_type_to_string(LibjType type);

/* Inverse function to libj_type_to_stirng().
 * libj_string_to_type(libj_type_to_string(type)) == type */
LibjType libj_string_to_type(const char *string);

/* Convert LibjError to const char *. Example: LIBJ_ERROR_OK -> "LIBJ_ERROR_OK". */
const char *libj_error_to_string(LibjError error);

/* Convert LibjError to const char * that's more human readable. Example: LIBJ_ERROR_OK -> "Success". */
const char *libj_error_to_human_readable_string(LibjError error);

/* Inverse function to libj_error_to_string().
 * libj_string_to_error(libj_error_to_string(err)) == err */
LibjError libj_string_to_error(const char *string);

/* Get type of json. json == NULL is not allowed. */
LibjError libj_type_of(Libj *libj, LibjJson *json, LibjType *type);

/* Release resources associated with json. *json == NULL is allowed. */
LibjError libj_free_json(Libj *libj, LibjJson **json);

/* Create a copy of source and place it into target. */
LibjError libj_copy(Libj *libj, LibjJson *source, LibjJson **target);

/**********************************************************************************
 * Object's functions
 **********************************************************************************/

/* Create empty json object. */
LibjError libj_object_create(Libj *libj, LibjJson **json);

/* TODO: This should be convenience. Implement this through object_get_member_at_ex(). */
LibjError libj_object_count_versions_ex(
        Libj *libj, LibjJson *json, const char *name, size_t name_size, size_t *nversions);

/* TODO: This should be convenience. Implement this through object_get_member_at_ex(). */
LibjError libj_object_get_version_ex(
        Libj *libj, LibjJson *json, LibjJson **value, const char *name, size_t name_size, size_t version);

/* Insert new member before specified position. */
LibjError libj_object_insert_at_ex(
        Libj *libj, LibjJson *json, size_t position, const char *name, size_t name_size, LibjJson *value);

LibjError libj_object_get_size(Libj *libj, LibjJson *json, size_t *size);

LibjError libj_object_get_member_at_ex(
        Libj *libj, LibjJson *json, size_t i, const char **name, size_t *name_size, LibjJson **value);

LibjError libj_object_remove_at(Libj *libj, LibjJson *json, size_t index);

/**********************************************************************************
 * Array's functions
 **********************************************************************************/

LibjError libj_array_create(Libj *libj, LibjJson **json);

/* TODO: This should be convenience. Make libj_array_insert_at essential(). */
LibjError libj_array_add(Libj *libj, LibjJson *json, LibjJson *element);

LibjError libj_array_get_size(Libj *libj, LibjJson *json, size_t *size);

LibjError libj_array_get_element_at(Libj *libj, LibjJson *json, size_t i, LibjJson **element);

LibjError libj_array_remove_at(Libj *libj, LibjJson *json, size_t index);

/**********************************************************************************
 * Primitive extractor functions
 **********************************************************************************/

LibjError libj_get_integer(Libj *libj, LibjJson *json, int64_t *value);

LibjError libj_get_real(Libj *libj, LibjJson *json, double *value);

LibjError libj_get_number(Libj *libj, LibjJson *json, char **value);

LibjError libj_get_bool(Libj *libj, LibjJson *json, bool *value);

LibjError libj_get_string_ex(Libj *libj, LibjJson *json, char **value, size_t *value_size);

/**********************************************************************************
 * Primitive create functions
 **********************************************************************************/

LibjError libj_string_create_ex(Libj *libj, LibjJson **json, const char *value, size_t value_size);

LibjError libj_integer_create(Libj *libj, LibjJson **json, int64_t value);

LibjError libj_real_create(Libj *libj, LibjJson **json, double value);

LibjError libj_number_create(Libj *libj, LibjJson **json, const char *value);

LibjError libj_bool_create(Libj *libj, LibjJson **json, bool value);

LibjError libj_null_create(Libj *libj, LibjJson **json);

/**********************************************************************************
 * String conversion functions
 **********************************************************************************/

/* Convert json into a string which can possibly have a '\0' byte. */
LibjError libj_to_string_ex(Libj *libj, LibjJson *json, char **json_string, size_t *json_string_size,
                            LibjToStringOptions *options);

LibjError libj_from_string_ex(Libj *libj, LibjJson **json,
                              const char *input_string, size_t input_size,
                              const char **error_string);

/* Parse json from byte sequence possibly containing '\0'. */
LibjError libj_from_input_stream(Libj *libj, LibjJson **json, LibisInputStream *input, const char **error_string);

#endif

