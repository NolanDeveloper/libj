#ifndef LIBJ_CONVENIENCE_H
#define LIBJ_CONVENIENCE_H

#include "libj_essential.h"

/* Options that may be passed io libj_to_string to get multiline string representation of json. */
extern LibjToStringOptions libj_to_string_options_pretty;

/* Options that may be passed io libj_to_string to get one line compact string representation of json. */
extern LibjToStringOptions libj_to_string_options_compact;


/* Add new field of type string to an object. If name of the field may contain '\0' prefer to use _ex functions. */
LibjError libj_object_add_string(Libj *libj, LibjJson *json, const char *name, const char *value);

LibjError libj_object_add_integer(Libj *libj, LibjJson *json, const char *name, intmax_t value);

LibjError libj_object_add_real(Libj *libj, LibjJson *json, const char *name, double value);

LibjError libj_object_add_number(Libj *libj, LibjJson *json, const char *name, const char *value);

LibjError libj_object_add_bool(Libj *libj, LibjJson *json, const char *name, bool value);

LibjError libj_object_add_null(Libj *libj, LibjJson *json, const char *name);

LibjError libj_object_add(Libj *libj, LibjJson *json, const char *name, LibjJson *value);

LibjError libj_object_count_versions(Libj *libj, LibjJson *json, const char *name, size_t *nversions);

/**********************************************************************************
 * Object's functions convenience
 **********************************************************************************/

LibjError libj_object_get_version(Libj *libj, LibjJson *json, LibjJson **value, const char *name, size_t version);

#define libj_object_get_string(...) libj_object_get_string_(__VA_ARGS__, NULL);

LibjError libj_object_get_string_(Libj *libj, LibjJson *json, char **value, const char *name, ...);

LibjError libj_object_get_string_v(Libj *libj, LibjJson *json, char **value, const char *name, va_list args);

#define libj_object_get_string_or_default(...) libj_object_get_string_or_default_(__VA_ARGS__, NULL)

LibjError libj_object_get_string_or_default_(
        Libj *libj, LibjJson *json, char **value, char *default_value, const char *name, ...);

LibjError libj_object_get_string_or_default_v(
        Libj *libj, LibjJson *json, char **value, char *default_value, const char *name, va_list args);

#define libj_object_get_integer(...) libj_object_get_integer_(__VA_ARGS__, NULL)

LibjError libj_object_get_integer_(Libj *libj, LibjJson *json, intmax_t *value, const char *name, ...);

LibjError libj_object_get_integer_v(Libj *libj, LibjJson *json, intmax_t *value, const char *name, va_list args);

#define libj_object_get_integer_or_default(...) libj_object_get_integer_or_default_(__VA_ARGS__, NULL)

LibjError libj_object_get_integer_or_default_(
        Libj *libj, LibjJson *json, intmax_t *value, intmax_t default_value, const char *name, ...);

LibjError libj_object_get_integer_or_default_v(
        Libj *libj, LibjJson *json, intmax_t *value, intmax_t default_value, const char *name, va_list args);

#define libj_object_get_real(...) libj_object_get_real_(__VA_ARGS__, NULL)

LibjError libj_object_get_real_(Libj *libj, LibjJson *json, double *value, const char *name, ...);

LibjError libj_object_get_real_v(Libj *libj, LibjJson *json, double *value, const char *name, va_list args);

#define libj_object_get_real_or_default(...) libj_object_get_real_or_default_(__VA_ARGS__, NULL)

LibjError libj_object_get_real_or_default_(
        Libj *libj, LibjJson *json, double *value, double default_value, const char *name, ...);

LibjError libj_object_get_real_or_default_v(
        Libj *libj, LibjJson *json, double *value, double default_value, const char *name, va_list args);

#define libj_object_get_number(...) libj_object_get_number_(__VA_ARGS__, NULL)

LibjError libj_object_get_number_(Libj *libj, LibjJson *json, char **value, const char *name, ...);

LibjError libj_object_get_number_v(Libj *libj, LibjJson *json, char **value, const char *name, va_list args);

#define libj_object_get_number_or_default(...) libj_object_get_number_or_default_(__VA_ARGS__, NULL)

LibjError libj_object_get_number_or_default_(
        Libj *libj, LibjJson *json, char **value, char *default_value, const char *name, ...);

LibjError libj_object_get_number_or_default_v(
        Libj *libj, LibjJson *json, char **value, char *default_value, const char *name, va_list args);

#define libj_object_get_bool(...) libj_object_get_bool_(__VA_ARGS__, NULL)

LibjError libj_object_get_bool_(Libj *libj, LibjJson *json, bool *value, const char *name, ...);

LibjError libj_object_get_bool_v(Libj *libj, LibjJson *json, bool *value, const char *name, va_list args);

#define libj_object_get_bool_or_default(...) libj_object_get_bool_or_default_(__VA_ARGS__, NULL)

LibjError libj_object_get_bool_or_default_(
        Libj *libj, LibjJson *json, bool *value, bool default_value, const char *name, ...);

LibjError libj_object_get_bool_or_default_v(
        Libj *libj, LibjJson *json, bool *value, bool default_value, const char *name, va_list args);

#define libj_object_get(...) libj_object_get_(__VA_ARGS__, NULL)

LibjError libj_object_get_(Libj *libj, LibjJson *json, LibjJson **value, const char *name, ...);

LibjError libj_object_get_v(Libj *libj, LibjJson *json, LibjJson **value, const char *name, va_list args);

LibjError libj_object_get_ex(Libj *libj, LibjJson *json, LibjJson **value, const char *name, size_t name_size);

LibjError libj_object_set_null(Libj *libj, LibjJson *json, const char *name);

LibjError libj_object_set_bool(Libj *libj, LibjJson *json, const char *name, bool value);

LibjError libj_object_set_integer(Libj *libj, LibjJson *json, const char *name, intmax_t value);

LibjError libj_object_set_real(Libj *libj, LibjJson *json, const char *name, double value);

LibjError libj_object_set_string(Libj *libj, LibjJson *json, const char *name, const char *value);

LibjError libj_object_set(Libj *libj, LibjJson *json, const char *name, LibjJson *value);

LibjError libj_object_set_ex(Libj *libj, LibjJson *json, const char *name, size_t name_size, LibjJson *value);

LibjError libj_object_get_member_at(Libj *libj, LibjJson *json, size_t i, const char **name, LibjJson **value);

#define libj_object_remove(...) libj_object_remove_(__VA_ARGS__, NULL)

LibjError libj_object_remove_(Libj *libj, LibjJson *json, const char *name, ...);

LibjError libj_object_remove_v(Libj *libj, LibjJson *json, const char *name, va_list args);

LibjError libj_object_remove_ex(Libj *libj, LibjJson *json, const char *name, size_t name_size, size_t version);

LibjError libj_object_remove_all_ex(Libj *libj, LibjJson *json, const char *name, size_t name_size);

/**********************************************************************************
 * Array's functions convenience
 **********************************************************************************/

LibjError libj_array_add_string(Libj *libj, LibjJson *json, const char *value);

LibjError libj_array_add_integer(Libj *libj, LibjJson *json, intmax_t value);

LibjError libj_array_add_real(Libj *libj, LibjJson *json, double value);

LibjError libj_array_add_number(Libj *libj, LibjJson *json, const char *value);

LibjError libj_array_add_bool(Libj *libj, LibjJson *json, bool value);

LibjError libj_array_add_null(Libj *libj, LibjJson *json);

/**********************************************************************************
 * Primitive extractor functions convenience
 **********************************************************************************/

LibjError libj_get_string(Libj *libj, LibjJson *json, char **value);

/**********************************************************************************
 * Primitive create functions convenience
 **********************************************************************************/

LibjError libj_string_create(Libj *libj, LibjJson **json, const char *value);

/**********************************************************************************
 * String conversion functions convenience
 **********************************************************************************/

/* Convert json into a string. */
LibjError libj_to_string(Libj *libj, LibjJson *json, char **json_string, LibjToStringOptions *options);

/* Parse json from string. */
LibjError libj_from_string(Libj *libj, LibjJson **json, const char *input_string, const char **error_string);

#endif
