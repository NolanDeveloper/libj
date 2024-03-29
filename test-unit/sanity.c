#include "test.h"

#define assert_not_null(pointer) \
    do { \
        if (NULL == (pointer)) { \
            printf("At " __FILE__ ":%d\n",  __LINE__); \
            printf("Assertion failed: value is NULL: %s\n", #pointer); \
            abort(); \
        } \
    } while (0)

#define assert_equal_int(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            printf("At " __FILE__ ":%d\n",  __LINE__); \
            printf("Assertion failed for expression: %s\n", #actual); \
            printf("Expected value: %d\n", (int) (expected)); \
            printf("Actual   value: %d\n", (int) (actual)); \
            abort(); \
        } \
    } while (0)

#define assert_equal_double(expected, actual) \
    do { \
        if (fabsl((expected) - (actual)) > 1e-6) { \
            printf("At " __FILE__ ":%d\n",  __LINE__); \
            printf("Assertion failed for expression: %s\n", #actual); \
            printf("Expected value: %lf\n", (double) (expected)); \
            printf("Actual   value: %lf\n", (double) (actual)); \
            abort(); \
        } \
    } while (0)

#define assert_equal_string(expected, actual) \
    do { \
        if (strcmp((expected), (actual))) { \
            printf("At " __FILE__ ":%d\n",  __LINE__); \
            printf("Assertion failed for expression: %s\n", #actual); \
            printf("Expected value: %s\n", (expected)); \
            printf("Actual   value: %s\n", (actual)); \
        } \
    } while (0)

static LibjJson *create_object(void) {
    LibjJson *object = NULL;
    E(libj_object_create(libj, &object));
    E(libj_object_add_real(libj, object, "x", 0.1));
    E(libj_object_add_real(libj, object, "y", 0.5));
    E(libj_object_add_real(libj, object, "z", 0.7));
    E(libj_object_add_real(libj, object, "w", 1.0));
    E(libj_object_add_real(libj, object, "big", DBL_MAX));
    E(libj_object_add_real(libj, object, "-big", -DBL_MAX));
    E(libj_object_add_real(libj, object, "min", DBL_MIN));
    E(libj_object_add_real(libj, object, "-min", -DBL_MIN));
    return object;
}

static LibjJson *create_array(void) {
    LibjJson *array = NULL;
    E(libj_array_create(libj, &array));
    E(libj_array_add_integer(libj, array, 1));
    E(libj_array_add_integer(libj, array, 2));
    E(libj_array_add_integer(libj, array, 3));
    E(libj_array_add_integer(libj, array, 42));
    E(libj_array_add_integer(libj, array, 13));
    E(libj_array_add_integer(libj, array, 22));
    return array;
}

LibjJson *create_json(void) {
    LibjJson *array = create_array();
    LibjJson *object = create_object();
    LibjJson *empty_array = NULL;
    LibjJson *empty_object = NULL;
    LibjJson *json = NULL;
    LibjJson *json_null;
    E(libj_null_create(libj, &json_null));
    E(libj_array_create(libj, &empty_array));
    E(libj_object_create(libj, &empty_object));
    E(libj_object_create(libj, &json));
    E(libj_object_add_string(libj, json, "name", "nolan"));
    E(libj_object_set_string(libj, json, "password", "querty"));
    E(libj_object_add_integer(libj, json, "age", 42));
    E(libj_object_add_real(libj, json, "pi", 3.14));
    E(libj_object_add_bool(libj, json, "bool", true));
    E(libj_object_add_null(libj, json, "null"));
    /* E(libj_object_insert_at_ex(libj, json, 6, "null1", 5, json_null)); */
    E(libj_object_add(libj, json, "empty_array", empty_array));
    E(libj_object_add(libj, json, "empty_object", empty_object));
    E(libj_object_add(libj, json, "array", array));
    E(libj_object_add(libj, json, "object", object));
    E(libj_free_json(libj, &json_null));
    E(libj_free_json(libj, &empty_array));
    E(libj_free_json(libj, &empty_object));
    E(libj_free_json(libj, &array));
    E(libj_free_json(libj, &object));
    return json;
}

void sanity_check(void) {
    LibjJson *empty_array;
    E(libj_array_create(libj, &empty_array));
    E(libj_array_add_integer(libj, empty_array, 42));
    E(libj_array_remove_at(libj, empty_array, 0));
    E(libj_free_json(libj, &empty_array));

    LibjJson *empty_object;
    E(libj_object_create(libj, &empty_object));
    E(libj_object_add_integer(libj, empty_object, "deleteme", 42));
    E(libj_object_remove_at(libj, empty_object, 0));
    E(libj_free_json(libj, &empty_object));

    LibjJson *json = create_json();

    char *json_string;
    E(libj_to_string(libj, json, &json_string, &libj_to_string_options_compact));
    assert_not_null(json_string);
    free(json_string);

    double number;

    E(libj_object_get_real(libj, json, &number, "age"));
    assert_equal_double(42.0, number);

    E(libj_object_get_real(libj, json, &number, "pi"));
    assert_equal_double(3.14, number);

    E(libj_object_get_real(libj, json, &number, "object", "x"));
    assert_equal_double(0.1, number);

    E(libj_object_get_real(libj, json, &number, "object", "y"));
    assert_equal_double(0.5, number);

    E(libj_object_get_real(libj, json, &number, "object", "z"));
    assert_equal_double(0.7, number);

    E(libj_object_get_real(libj, json, &number, "object", "big"));
//    assert(DBL_MAX == number);

    E(libj_object_get_real(libj, json, &number, "object", "-big"));
//    assert(-DBL_MAX == number);

    E(libj_object_get_real(libj, json, &number, "object", "min"));
//    assert(DBL_MIN == number);

    E(libj_object_get_real(libj, json, &number, "object", "-min"));
//    assert(-DBL_MIN == number);

    LibjJson *json_copy;
    LibjJson *json_from_string;
    E(libj_copy(libj, json, &json_copy));
    char *str1, *str2;
    E(libj_to_string(libj, json_copy, &str1, &libj_to_string_options_pretty));
    printf("%s\n", str1);
    const char *error_string;
    E(libj_from_string(libj, &json_from_string, str1, &error_string));
    E(libj_to_string(libj, json_from_string, &str2, &libj_to_string_options_pretty));
    assert_equal_string(str1, str2);
    free(str1);
    free(str2);
    E(libj_free_json(libj, &json_from_string));

    E(libj_object_remove(libj, json_copy, "null"));
    E(libj_to_string(libj, json_copy, &json_string, &libj_to_string_options_pretty));
    free(json_string);

    E(libj_free_json(libj, &json));
    E(libj_free_json(libj, &json_copy));

    LibjJson *json_with_escape;

    const char str[] = "\"null \\n \\f \\b \\uD83D\\uDE0E \\u0000 \"";
    const char value_str[] = "null \n \f \b 😎 \0 ";
    E(libj_from_string(libj, &json_with_escape, str, &error_string));
    E(libj_to_string(libj, json_with_escape, &json_string, &libj_to_string_options_pretty));
    char *value;
    size_t value_size;
    E(libj_get_string_ex(libj, json_with_escape, &value, &value_size));
    assert_equal_int(sizeof(value_str) - 1, value_size);
    assert(!memcmp(value, value_str, value_size));

    E(libj_free_json(libj, &json_with_escape));
    free(json_string);
}

