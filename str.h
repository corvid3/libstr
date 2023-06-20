#pragma once

/*

c string implementation in pure C

AUTHOR: korvo
    (korvonesto.com)

*/

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef STR_INLINE
#define STR_EXPORT static inline
#else
#define STR_EXPORT extern
#endif

typedef struct {
    char* ptr;

    uint32_t capacity;
    uint32_t len;
} str_t;

#define STR_STATIC_DEFN(str) \
    (str_t) { .ptr = str, .capacity = sizeof(str) - 1, .len = sizeof(str) - 1 }

#define STR_IS_NULL(str) (str.ptr == NULL)
#define STR_MAKE_NULL \
    (str_t) { .ptr = NULL, .capacity = 0, .len = 0 }

STR_EXPORT str_t str_create();
STR_EXPORT str_t str_generate(const char* format, ...);
STR_EXPORT str_t str_clone(const str_t* from);
STR_EXPORT str_t str_from_cstr_move(char**);
STR_EXPORT str_t str_from_cstr_clone(const char*);
// end must be one after whatever you want to copy
STR_EXPORT str_t str_from_range(const char* begin, const char* end);
STR_EXPORT const char* str_cstr(const str_t*);
STR_EXPORT void str_destroy(str_t*);
STR_EXPORT bool str_cmp(const str_t*, const str_t*);
STR_EXPORT bool str_cmp_cstr(const str_t*, const char*);
STR_EXPORT void str_append_char(str_t*, const char);
STR_EXPORT void str_append_cstr(str_t*, const char*);
STR_EXPORT void str_append(str_t*, const str_t* from);
STR_EXPORT void str_insert_char(str_t*, const char, const uint32_t idx);
STR_EXPORT void str_insert(str_t*, const str_t* from, const uint32_t idx);
STR_EXPORT char str_pop(str_t*);

#ifndef DEFAULT_STR_CAPACITY
#define DEFAULT_STR_CAPACITY 32
#endif

#ifndef DEFAULT_STR_GROWTH_FACTOR
#define DEFAULT_STR_GROWTH_FACTOR 1.5f
#endif

#ifdef STR_IMPLEMENTATION

#define STR_UNUSED(x) (void)x

static inline uint32_t str_cstrlen(const char* in) {
    uint32_t len = 0;
    while (in[len] != 0)
        len += 1;
    return len;
}

static inline void str_memcpy(char* to, const char* from, uint32_t len) {
    for (uint32_t idx = 0; idx < len; idx++)
        to[idx] = from[idx];
}

// continuously resizes a strings capacity until it is >= than fit_cap
static inline void str_fit(str_t* str, uint32_t fit_cap) {
    uint32_t cap = str->capacity;

    while (cap <= fit_cap) {
        cap *= DEFAULT_STR_GROWTH_FACTOR;
    }

    str->capacity = cap;
    str->ptr = realloc(str->ptr, cap);
}

STR_EXPORT str_t str_create() {
    const str_t out = {
        .ptr = malloc(DEFAULT_STR_CAPACITY),
        .capacity = DEFAULT_STR_CAPACITY,
        .len = 0,
    };

    return out;
}

STR_EXPORT str_t str_generate(const char* fmt, ...) {
    va_list list, copy;
    va_start(list, fmt);
    va_copy(copy, list);

    uint32_t len = vsnprintf(NULL, 0, fmt, list);
    char* ptr = malloc(len + 1);
    ptr[len] = 0;
    vsnprintf(ptr, len + 1, fmt, copy);

    va_end(list);
    va_end(copy);

    return (str_t){
        .ptr = ptr,
        .len = len,
        .capacity = len,
    };
}

STR_EXPORT str_t str_clone(const str_t* in) {
    char* copy = malloc(in->len + 1);
    copy[in->len] = 0;
    str_memcpy(copy, in->ptr, in->len);
    return (str_t){
        .len = in->len,
        .capacity = in->len,
        .ptr = copy,
    };
}

STR_EXPORT const char* str_cstr(const str_t* in) {
    if (in->ptr[in->len] != 0)
        in->ptr[in->len] = 0;

    return in->ptr;
}

// clones the cstr
STR_EXPORT str_t str_from_cstr_clone(const char* in) {
    const uint32_t len = str_cstrlen(in);

    char* clone = malloc(len);
    for (uint32_t i = 0; i < len; i++)
        clone[i] = in[i];

    const str_t out = {
        .ptr = clone,
        .capacity = len,
        .len = len,
    };

    return out;
}

STR_EXPORT str_t str_from_cstr_move(char** in) {
    char* cstr = *in;
    *in = NULL;

    const uint32_t len = str_cstrlen(cstr);
    const str_t out = {
        .ptr = cstr,
        .capacity = len,
        .len = len,
    };

    return out;
}

STR_EXPORT str_t str_from_range(const char* begin, const char* end) {
    const uint32_t len = end - begin;
    char* copy = malloc(len);
    str_memcpy(copy, begin, len);
    return (str_t){
        .ptr = copy,
        .capacity = len,
        .len = len,
    };
}

STR_EXPORT void str_destroy(str_t* str) {
    if (str->ptr)
        free(str->ptr);
}

STR_EXPORT bool str_cmp(const str_t* left, const str_t* right) {
    if (left->len != right->len)
        return false;

    for (uint32_t i = 0; i < left->len; i++)
        if (left->ptr[i] != right->ptr[i])
            return false;

    return true;
}

STR_EXPORT bool str_cmp_cstr(const str_t* left, const char* right) {
    const uint32_t right_len = str_cstrlen(right);

    if (left->len != right_len)
        return false;

    for (uint32_t i = 0; i < left->len; i++)
        if (left->ptr[i] != right[i])
            return false;

    return true;
}

STR_EXPORT void str_append_char(str_t* str, char c) {
    str_fit(str, str->len + 1);
    str->ptr[str->len] = c;
    str->len += 1;
}

STR_EXPORT void str_append_cstr(str_t* str, const char* from) {
    const uint32_t from_len = str_cstrlen(from);
    const uint32_t final_len = str->len + from_len;

    str_fit(str, final_len);
    for (uint32_t i = 0; i < from_len; i++)
        str->ptr[i + str->len] = from[i];

    str->len += from_len;
}

STR_EXPORT void str_append(str_t* str, const str_t* from) {
    const uint32_t final_len = str->len + from->len;

    str_fit(str, final_len);

    for (uint32_t i = 0; i < from->len; i++)
        str->ptr[i + str->len] = from->ptr[i];

    str->len += from->len;
}

STR_EXPORT void str_insert_char(str_t* str, const char c, const uint32_t idx) {
    str_fit(str, str->len += 1);

    for (uint32_t i = str->len; i > idx; i--)
        str->ptr[i] = str->ptr[i - 1];

    str->ptr[idx] = c;
}

STR_EXPORT void str_insert(str_t* str, const str_t* from, const uint32_t idx) {
    const uint32_t final_len = str->len + from->len;

    str_fit(str, final_len);

    // move all of the characters that are displaced over
    for (uint32_t i = final_len; i >= idx; i--)
        str->ptr[i] = str->ptr[i - from->len];

    // now copy the other string into its new place
    for (uint32_t i = str->len; i < final_len; i++)
        str->ptr[i] = from->ptr[i];

    str->len = final_len;
}

#endif
