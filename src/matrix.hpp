#pragma once

#include "types.hpp"

// C = A*B, A nxp, B pxm, C nxm
void baseline_matrix_multiplication(const uint32_t n, const uint32_t p, const uint32_t m, const data_t* a, const data_t* b, data_t* c);

// C = A*A^T
void baseline_multiply_matrix_transpose(const uint32_t n, const uint32_t m, const data_t* a, data_t* c);

// C = A^T*A
void baseline_multiply_transpose_matrix(const uint32_t n, const uint32_t m, const data_t* a, data_t* c);

// A nxm, A^T mxn
void transpose(const uint32_t n, const uint32_t m, const data_t* a, data_t* a_t);
