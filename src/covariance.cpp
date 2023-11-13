#include "covariance.hpp"
#include "types.hpp"

static void read_band(const uint32_t pixels, stream<data_t> &data, data_t out[MAX_PIXELS]) {
  assert(pixels % 16 == 0);
  for (uint32_t px = 0; px < pixels; px++) {
    out[px] = data.read();
  }
}

static void multiply_band_with_matrix(uint32_t pixels, uint16_t bands, data_t crnt_band[MAX_PIXELS], stream<data_t> &data, stream<data_t> &covar) {
  assert(pixels % 16 == 0);
  assert(bands % 8 == 0);
  for (uint16_t band_rgt = 0; band_rgt < bands; band_rgt++) {
    data_t dot_product = 0;
    for (uint32_t px = 0; px < MAX_PIXELS; px += 4) {
      #pragma HLS pipeline II=4
      if (px < pixels) {
        data_t v0 = crnt_band[px+0] * data.read();
        data_t v1 = crnt_band[px+1] * data.read();
        data_t v2 = crnt_band[px+2] * data.read();
        data_t v3 = crnt_band[px+3] * data.read();

        dot_product += (v0 + v2) + (v1 + v3);
      }
    }
    const data_t covar_val = dot_product / pixels - 1;
    covar.write(dot_product);
  }
}

// Instead of multiplying the bsq matrix by the transpose, we multiply one band with the bsq matrix at a time.
// Not very good since we need to store a whole band in ram, which is larger that the covar matrix
void covariance_bsq(uint32_t pixels, uint16_t bands, stream<data_t> &data1, stream<data_t> &data2, stream<data_t> &covar) {
  assert(pixels % 16 == 0);
  assert(bands % 8 == 0);

  for (uint16_t band_lft = 0; band_lft < bands; band_lft++) {
    #pragma HLS unroll off=true
    data_t crnt_band[MAX_PIXELS];
    read_band(pixels, data1, crnt_band);
    multiply_band_with_matrix(pixels, bands, crnt_band, data2, covar);
  }
}

// TODO: covariance_bip can be done by reading one pixels, multiplying itself into a matrix and adding them all up to make the covariance.
// This could be parallelized between pixels (read a couple of pixels, multiply them in parallel, add them to the covariance matrix)


void covariance_bsq_ip(stream<data_t> &data1, stream<data_t> &data2, stream<data_t> &covar) {
  covariance_bsq(MAX_PIXELS, MAX_BANDS, data1, data2, covar);
}