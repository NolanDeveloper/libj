#ifndef LIBJ_INTERNAL_H
#define LIBJ_INTERNAL_H

#include <uchar.h>

#include <libj.h>
#include <libsb.h>
#include <libgb.h>
#include <libis.h>

struct Libj_ {
    Libsb *libsb;
    Libgb *libgb;
    Libis *libis;
    LibjToStringOptions *to_string_options;
    LibsbBuilder *builder;
    int depth;
    char *error_string;
};

typedef struct {
    size_t size;
    char *value;
} LibjString;

typedef struct {
    LibjString name;
    LibjJson *value;
} LibjMember;

typedef struct {
    size_t size;
    LibjMember *members;
} LibjObject;

typedef struct {
    size_t size;
    LibjJson **elements;
} LibjArray;

struct LibjJson_ {
    LibjType type;
    union {
        LibjObject object;
        LibjArray array;
        LibjString string;
        bool boolean;
    };
};

#define ESB libsberror_to_libjerror

LibjError libsberror_to_libjerror(LibsbError err);

#define EIS libiserror_to_libjerror

LibjError libiserror_to_libjerror(LibisError err);

#define EGB libgberror_to_libjerror

LibjError libgberror_to_libjerror(LibgbError err);

#define E libj_handle_internal_error

LibjError libj_handle_internal_error(LibjError err);

LibjError libj_parse_value(Libj *libj, LibjJson **json, LibisInputStream *input);

LibjError libj_parse_value_object(Libj *libj, LibjJson **json, LibisInputStream *input);

LibjError libj_parse_value_array(Libj *libj, LibjJson **json, LibisInputStream *input);

LibjError libj_parse_value_true(Libj *libj, LibjJson **json, LibisInputStream *input);

LibjError libj_parse_value_false(Libj *libj, LibjJson **json, LibisInputStream *input);

LibjError libj_parse_value_null(Libj *libj, LibjJson **json, LibisInputStream *input);

LibjError libj_parse_value_string(Libj *libj, LibjJson **json, LibisInputStream *input);

#endif
