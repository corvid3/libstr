#include "libtest/test.h"
#include <string.h>

#define STR_IMPLEMENTATION
#include "str.h"

static const char* strcmp_test() {
    str_t str1 = str_from_cstr_clone("hello");
    str_t str2 = str_from_cstr_clone("hello");
    str_t str3 = str_from_cstr_clone("hi there");

    if (!str_cmp(&str1, &str2)) {
        return TESTC_BASIC_ERR;
    }

    if (!str_cmp_cstr(&str1, "hello"))
        return TESTC_BASIC_ERR;

    str_destroy(&str1);
    str_destroy(&str2);
    str_destroy(&str3);
    return NULL;
}

static const char* appending_test() {
    str_t str1 = str_from_cstr_clone("hello, ");
    str_t str2 = str_from_cstr_clone("there!");
    str_t expected = str_from_cstr_clone("hello, there!");

    str_append(&str1, &str2);

    if (!str_cmp(&str1, &expected)) {
        return TESTC_BASIC_ERR;
    }

    str_destroy(&str1);
    str_destroy(&str2);
    str_destroy(&expected);

    return NULL;
}

static const char* move_test() {
    const char* _from = "hello";
    char* from = malloc(sizeof "hello");
    strcpy(from, _from);

    str_t expected = str_from_cstr_clone("hello");

    str_t into = str_from_cstr_move(&from);
    if (!str_cmp(&into, &expected)) {
        return TESTC_BASIC_ERR;
    }

    str_destroy(&expected);
    str_destroy(&into);

    return NULL;
}

static const char* static_test() {
    const str_t str = STR_STATIC_DEFN("hello");
    str_t str_2 = str_from_cstr_clone("hello");

    if (!str_cmp(&str, &str_2)) {
        return TESTC_BASIC_ERR;
    }

    str_destroy(&str_2);
    return NULL;
}

static const char* append_cstr() {
    str_t str = str_from_cstr_clone("hi, ");
    str_append_cstr(&str, "there!");

    str_t str_2 = str_from_cstr_clone("hi, there!");

    if (!str_cmp(&str, &str_2)) {
        return TESTC_BASIC_ERR;
    }

    str_destroy(&str);
    str_destroy(&str_2);
    return NULL;
}

static const char* from_range() {
    char* data = "hello, world!";

    str_t str = str_from_range(&data[7], &data[12]);

    if (!str_cmp_cstr(&str, "world"))
        return TESTC_BASIC_ERR;

    str_destroy(&str);

    return NULL;
}

static const char* cloning() {
    const str_t from = STR_STATIC_DEFN("hello");
    str_t clone = str_clone(&from);
    if (!str_cmp(&clone, &from))
        return TESTC_BASIC_ERR;
    str_destroy(&clone);
    return NULL;
}

static const char* generate_test() {
    str_t into = str_generate("%s", "hi");
    if (!str_cmp_cstr(&into, "hi"))
        return TESTC_BASIC_ERR;
    return NULL;
}

const test_t tests[] = {
    (test_t){
        .ptr = strcmp_test,
        .name = "strcmp",
        .desc = "",
    },

    (test_t){
        .ptr = appending_test,
        .name = "appending",
        .desc = "",
    },

    (test_t){
        .ptr = move_test,
        .name = "moving cstrs",
        .desc = "",
    },

    (test_t){
        .ptr = static_test,
        .name = "static test",
        .desc = "",
    },

    (test_t){
        .ptr = append_cstr,
        .name = "append cstr",
        .desc = "",
    },

    (test_t){
        .ptr = from_range,
        .name = "from range",
        .desc = "",
    },

    (test_t){
        .ptr = cloning,
        .name = "cloning",
        .desc = "",
    },

    (test_t){
        .ptr = generate_test,
        .name = "generate",
        .desc = "",
    },
};

extern int main() {
    bool passed = true;

    for (uint32_t i = 0; i < sizeof(tests) / sizeof(test_t); i++) {
        test_t test = tests[i];
        passed &= execute_test(test);
    }

    printf("\n");

    if (!passed) {
        printf("SANITY CHECK FAILED\n");
    } else {
        printf("sanity check passed\n");
    }
}
