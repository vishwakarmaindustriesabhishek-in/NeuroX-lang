#ifndef NEUROX_COMMON_H
#define NEUROX_COMMON_H

// POSIX feature for strdup
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Version
#define NEUROX_VERSION_MAJOR 0
#define NEUROX_VERSION_MINOR 1
#define NEUROX_VERSION_PATCH 0

// Memory management
#define NEUROX_MALLOC(size) malloc(size)
#define NEUROX_REALLOC(ptr, size) realloc(ptr, size)
#define NEUROX_FREE(ptr) free(ptr)

// String utilities
#define NEUROX_STRDUP(s) strdup(s)

// Error handling
typedef enum {
    NEUROX_OK = 0,
    NEUROX_ERROR_MEMORY,
    NEUROX_ERROR_SYNTAX,
    NEUROX_ERROR_TYPE,
    NEUROX_ERROR_SEMANTIC,
    NEUROX_ERROR_IO,
    NEUROX_ERROR_RUNTIME,
} neurox_error_t;

// Diagnostic reporting
typedef struct {
    const char *filename;
    int line;
    int column;
    const char *message;
    neurox_error_t error_code;
} neurox_diagnostic_t;

void neurox_report_error(neurox_diagnostic_t *diag);
void neurox_report_warning(neurox_diagnostic_t *diag);

// Dynamic array utilities
#define NEUROX_ARRAY_INIT_CAPACITY 16

#define NEUROX_ARRAY_DEFINE(name, type) \
    typedef struct { \
        type *data; \
        size_t count; \
        size_t capacity; \
    } name##_array_t; \
    \
    static inline void name##_array_init(name##_array_t *arr) { \
        arr->data = NULL; \
        arr->count = 0; \
        arr->capacity = 0; \
    } \
    \
    static inline void name##_array_push(name##_array_t *arr, type item) { \
        if (arr->count >= arr->capacity) { \
            size_t new_cap = arr->capacity == 0 ? NEUROX_ARRAY_INIT_CAPACITY : arr->capacity * 2; \
            arr->data = NEUROX_REALLOC(arr->data, new_cap * sizeof(type)); \
            arr->capacity = new_cap; \
        } \
        arr->data[arr->count++] = item; \
    } \
    \
    static inline void name##_array_free(name##_array_t *arr) { \
        NEUROX_FREE(arr->data); \
        arr->data = NULL; \
        arr->count = 0; \
        arr->capacity = 0; \
    }

#endif // NEUROX_COMMON_H
