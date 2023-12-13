#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <libsb.h>
#include <libj.h>
#include <ctype.h>

static Libsb *libsb;
static Libis *libis;
static Libj *libj;

static int number_of_tests;
static int number_of_tests_passed;

static void run_test(const char *path, bool should_parse, const char *json_string, size_t json_string_size) {
    LibisSource *source = NULL;
    LibisInputStream *input = NULL;
    LibjJson *json = NULL;
    bool success = true;
    const char *error_string = NULL;
    char c;
    bool eof;
    LibisError err_is = libis_source_create_from_buffer(libis, &source, json_string, json_string_size, false);
    if (err_is) {
        success = false;
        goto end;
    }
    err_is = libis_create(libis, &input, &source, 1);
    if (err_is) {
        success = false;
        goto end;
    }
    LibjError err_j = libj_from_input_stream(libj, &json, input, &error_string);
    err_is = libis_lookahead(libis, input, &eof, 1, &c);
    if (err_is) {
        success = false;
        goto end;
    }
    while (isspace(c)) {
        err_is = libis_skip_char(libis, input, &eof, &c);
        if (err_is) {
            success = false;
            goto end;
        }
    }
    bool is_parsed = err_j == LIBJ_ERROR_OK;
    bool is_parsed_completely = is_parsed && eof;
    bool has_error_message = error_string && strcmp(error_string, "");
    if (!is_parsed && !has_error_message) {
        success = false;
        goto end;
    }
    if (should_parse != is_parsed_completely) {
        success = false;
        goto end;
    }
end:
    libj_free_json(libj, &json);
    libis_source_destroy(libis, &source);
    libis_destroy(libis, &input);
    ++number_of_tests;
    if (success) {
        ++number_of_tests_passed;
    } else {
        printf("FAIL %s\n", path);
    }
}

static bool read_file(const char *filename, char **bytes, size_t *size) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        return false;
    }
    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    fseek(f, 0, SEEK_SET);
    *bytes = malloc(*size + 1);
    if (!*bytes) {
        fclose(f);
        return false;
    }
    size_t n = fread(*bytes, 1, *size, f);
    if (n != *size) {
        free(*bytes);
        *bytes = NULL;
        fclose(f);
        return false;
    }
    fclose(f);
    (*bytes)[*size] = '\0';
    return true;
}

int main(int argc, char *argv[]) {
    int err = EXIT_SUCCESS;
    char *path = NULL;
    size_t path_size = 0;
    char *content = NULL;
    DIR *json_test_suite_dir = NULL;
    LibsbBuilder *builder = NULL;
    if (argc != 2) {
        printf("Usage: %s ./JSONTestSuite\n", argv[0]);
        goto end;
    }
    if (LIBSB_ERROR_OK != libsb_start(&libsb)) {
        printf("libsb_start failed\n");
        goto end;
    }
    if (LIBIS_ERROR_OK != libis_start(&libis)) {
        printf("libis_start failed\n");
        goto end;
    }
    if (LIBJ_ERROR_OK != libj_start(&libj)) {
        printf("libj_start failed\n");
        goto end;
    }
    const char *json_test_suite_path = argv[1];
    json_test_suite_dir = opendir(json_test_suite_path);
    if (!json_test_suite_dir) {
        printf("Failed to open directory: %s\n", json_test_suite_path);
        goto end;
    }
    struct dirent *entry;
    while ((entry = readdir(json_test_suite_dir))) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
            continue;
        }
        const char *extension = strrchr(entry->d_name, '.');
        if (!extension || strcmp(extension, ".json") || (entry->d_name[0] != 'n' && entry->d_name[0] != 'y')) {
            continue;
        }
        LibsbError e = libsb_create(libsb, &builder);
        if (LIBSB_ERROR_OK != e) {
            goto end;
        }
        e = libsb_append(libsb, builder, "%s/%s", json_test_suite_path, entry->d_name);
        if (LIBSB_ERROR_OK != e) {
            goto end;
        }
        e = libsb_destroy_into(libsb, &builder, &path, &path_size);
        if (LIBSB_ERROR_OK != e) {
            goto end;
        }
        size_t content_size;
        if (!read_file(path, &content, &content_size)) {
            goto end;
        }
        run_test(path, entry->d_name[0] != 'n', content, content_size);
        free(path);
        free(content);
        path = NULL;
        content = NULL;
    }
    if (number_of_tests_passed != number_of_tests) {
        goto end;
    }
    printf("Success!\n");
    err = EXIT_SUCCESS;
end:
    if (json_test_suite_dir) {
        closedir(json_test_suite_dir);
    }
    libsb_destroy(libsb, &builder);
    free(path);
    free(content);
    libsb_finish(&libsb);
    libj_finish(&libj);
    return err;
}
