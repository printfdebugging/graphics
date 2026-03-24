#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include "cglm/cglm.h"

#define DECLARE_PRINT_MATRIX(n) \
    void print_mat##n(mat##n mat)

#define DECLARE_PRINT_VECTOR(n) \
    void print_vec##n(vec##n vec)

/*
 * NOTE: matrix element coordinates work a bit differently
 *       from the x-y coordiates. element42 in a matrix refers
 *       to the element in the 4th row and 2nd column, whereas
 *       in the coordinate system it refers to the element in
 *       the 4th column and second row. matrix indices start
 *       at 1 as opposed to 0 of the coordinate system.
 */

#define DEFINE_PRINT_MATRIX(n)                            \
    DECLARE_PRINT_MATRIX(n) {                             \
        for (int row = 0; row < n; ++row) {               \
            for (int col = 0; col < n; ++col) {           \
                fprintf(stderr, "%.1f, ", mat[col][row]); \
            }                                             \
            fprintf(stderr, "\n");                        \
        }                                                 \
        fprintf(stderr, "\n");                            \
    }

#define DEFINE_PRINT_VECTOR(n)                \
    DECLARE_PRINT_VECTOR(n) {                 \
        for (int i = 0; i < n; ++i) {         \
            fprintf(stderr, "%.1f ", vec[i]); \
        }                                     \
        fprintf(stderr, "\n");                \
    }

DECLARE_PRINT_MATRIX(4);
DECLARE_PRINT_VECTOR(4);

#endif
