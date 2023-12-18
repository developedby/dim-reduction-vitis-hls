#include "types.hpp"

// A nxp, B pxm, C nxm
void baseline_matrix_multiplication(const uint32_t n, const uint32_t p, const uint32_t m, data_t* a, data_t* b, data_t* c) {
  for (uint32_t a_row = 0; a_row < n; a_row++) {
    for (uint32_t b_col = 0; b_col < m; b_col++) {
      c[a_row*m + b_col] = 0;
      for (uint32_t i = 0; i < p; i++) {
        c[a_row*m + b_col] += a[a_row*p + i] * b[i*m + b_col];
      }
    }
  }
}

// C = A*A^T
void baseline_multiply_matrix_transpose(const uint32_t n, const uint32_t m, const data_t* a, data_t* c) {
  for (uint32_t row1 = 0; row1 < n; row1++) {
    for (uint32_t row2 = 0; row2 < n; row2++) {
      c[row1*m + row2] = 0;
      for (uint32_t i = 0; i < m; i++) {
        c[row1*m + row2] += a[row1*m + i] * a[row2*m + i];
      }
    }
  }
}

// C = A^T*A
void baseline_multiply_transpose_matrix(const uint32_t n, const uint32_t m, const data_t* a, data_t* c) {
  for (uint32_t i = 0; i < n*n; i++) {
    c[i] = 0;
  }

  for (uint32_t row = 0; row < n; row++) {
    for (uint32_t col1 = 0; col1 < m; col1++) {
      for (uint32_t col2 = 0; col2 < m; col2++) {
        c[col1*m + col2] = a[row*m + col1] * a[row*m + col2];
      }
    }
  }
}

// A nxm, A^T mxn
void transpose(const uint32_t n, const uint32_t m, data_t* a, data_t* a_t) {
  for (uint32_t i = 0; i < n; i++) {
    for (uint32_t j = 0; j < m; j++) {
      a_t[j*n + i] = a[i*m + j];
    }
  }
}