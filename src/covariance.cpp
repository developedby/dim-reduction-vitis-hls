#include "covariance.hpp"
#include "types.hpp"
#include "matrix.hpp"

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
    for (uint32_t px = 0; px < pixels; px += 4) {
      #pragma HLS pipeline II=4
      data_t v0 = crnt_band[px+0] * data.read();
      data_t v1 = crnt_band[px+1] * data.read();
      data_t v2 = crnt_band[px+2] * data.read();
      data_t v3 = crnt_band[px+3] * data.read();
      dot_product += (v0 + v2) + (v1 + v3);
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
    data_t crnt_band[MAX_PIXELS];
    read_band(pixels, data1, crnt_band);
    multiply_band_with_matrix(pixels, bands, crnt_band, data2, covar);
  }
}


void covariance_bsq_mem(const uint32_t pixels, const uint16_t bands, const data_t* data, data_t* covar) {
  const data_t pixels_m1 = data_t(1) / data_t(pixels - 1);
  const uint16_t blks = pixels / uint32_t(READ_BUF_LEN);
  const uint16_t blk_rem = pixels % READ_BUF_LEN;
  data_t buf1[READ_BUF_LEN];
  data_t buf2[READ_BUF_LEN];

  for (uint16_t band_lft = 0; band_lft < bands; band_lft++) {
    for (uint16_t band_rgt = 0; band_rgt < bands; band_rgt++) {
      data_t sum = 0;
      for (uint32_t pixel = 0; pixel < blks*READ_BUF_LEN; pixel += READ_BUF_LEN) {
        copy_arr<mem_unroll>(READ_BUF_LEN, &data[band_lft*pixels + pixel], &buf1[0]);
        copy_arr<mem_unroll>(READ_BUF_LEN, &data[band_rgt*pixels + pixel], &buf2[0]);
        sum += dot_product<op_unroll>(READ_BUF_LEN, buf1, buf2) * pixels_m1;
      }
      copy_arr<mem_unroll>(blk_rem, &data[band_lft*pixels + pixel], &buf1[0]);
      copy_arr<mem_unroll>(blk_rem, &data[band_rgt*pixels + pixel], &buf2[0]);
      sum += dot_product<op_unroll>(blk_rem, buf1, buf2);
      covar[band_lft*bands + band_rgt] = sum;
    }
  }
}

// TODO: covariance_bip can be done by reading one pixels, multiplying itself into a matrix and adding them all up to make the covariance.
// This could be parallelized between pixels (read a couple of pixels, multiply them in parallel, add them to the covariance matrix)
void covariance_bip_buf(const uint32_t pixels, const uint16_t bands, const data_t* data, data_t* covar) {
  assert(bands % 8 == 0);
  LOOP_CLEAR_COVAR:
  for (uint32_t i = 0; i < bands*bands; i++) {
    covar[i] = 0;
  }

  LOOP_COVAR_PIXELS:
  for (uint32_t px = 0; px < pixels; px++) {
    data_t pixel_data1[MAX_BANDS];
    data_t pixel_data2[MAX_BANDS];
    LOOP_COPY_PX:
    for (uint16_t band = 0; band < bands; band++) {
      const data_t val = data[px*bands + band];
      pixel_data1[band] = val;
      pixel_data2[band] = val;
    }

    LOOP_MUL_BANDS:
    for (uint16_t i = 0; i < bands; i++) {
      for (uint16_t j = 0; j < bands; j++) {
        covar[i*bands + j] += pixel_data1[i] * pixel_data2[j];
      }
    }
  }

  const data_t pixels_m1 = data_t(1) / data_t(pixels - 1);
  LOOP_DIV_COVAR:
  for (uint32_t i = 0; i < bands*bands; i++) {
    covar[i] *= pixels_m1;
  }
}

void covariance_bip_mem(const uint32_t pixels, const uint16_t bands, const data_t* data, data_t* covar) {
  baseline_multiply_transpose_matrix(pixels, bands, data, covar);
}

void covariance_bsq_mem(const uint32_t pixels, const uint16_t bands, const data_t* data, data_t* covar) {
  baseline_multiply_matrix_transpose(pixels, bands, data, covar);
}


/* For comparing synthesis */

void covariance_bip_ip(const data_t* data, data_t* covar) {
  covariance_bip(MAX_BANDS*10, MAX_BANDS, data, covar);
}

void covariance_bsq_ip(stream<data_t> &data1, stream<data_t> &data2, stream<data_t> &covar) {
  covariance_bsq(MAX_PIXELS, MAX_BANDS, data1, data2, covar);
}

void covariance_bsq_lib_ip() {
  
}