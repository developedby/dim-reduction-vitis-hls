#include "covariance.hpp"
#include "types.hpp"

void covariance_bsp(int pixels, int bands, hls::stream<data_t> &data, data_t covar[MAX_BANDS][MAX_BANDS]) {
  data_t elem_buf[MAX_BANDS];

  loop_mul:
  for (int i = 0; i < MAX_PIXELS; i++) {
    if (i < pixels) {
      for (int d = 0; d < MAX_BANDS; d++) {
        if (d < dim) {
          for (int d2 = 0; d2 < MAX_BANDS; d2++) {
            if (d2 < bands) {
              if (d == 0) {
                elem_buf[d2] = data.read();
              }
              covar[d][d2] += elem_buf[d] * elem_buf[d2];
            }
          }
        }
      }
    }
  }

  loop_div:
  for (int i = 0; i < MAX_BANDS; i++) {
    if (i < bands) {
      for (int j = 0; j < MAX_BANDS; j++) {
        if (j < bands) {
          covar[i][j] = data_t(covar[i][j] / (bands - 1));
        }
      }
    }
  }
}
