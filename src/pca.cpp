#include "types.hpp"
#include "pca.hpp"
#include "centering.hpp"
#include "covariance.hpp"
#include "svd.hpp"

void pca_bsq(const uint32_t pixels, const uint16_t bands, data_t* data_in, data_t* data_tmp) {
  const uint32_t points = pixels * bands;
  const uint32_t covar_size = bands * bands;

  stream<data_t> data1;
  for (uint32_t pt = 0; pt < points; pt++) {
    data1.write(data_in[pt]);
  }
  stream<data_t> data2;
  for (uint32_t pt = 0; pt < points; pt++) {
    data2.write(data_in[pt]);
  }

  stream<data_t> centered;
  mean_centering_bsq(pixels, bands, data1, data2, centered);
  for (uint32_t pt = 0; pt < points; pt++) {
    data_tmp[pt] = centered.read();
  }

  stream<data_t> centered1;
  for (uint32_t pt = 0; pt < points; pt++) {
    centered1.write(data_tmp[pt]);
  }
  stream<data_t> centered2;
  for (uint16_t band_lft = 0; band_lft < bands; band_lft++) {
    for (uint32_t pt = 0; pt < points; pt++) {
      centered2.write(data_tmp[pt]);
    }
  }

  data_t covariance[MAX_BANDS*MAX_BANDS];
  stream<data_t> covariance_stream;
  covariance_bsq(pixels, bands, centered1, centered2, covariance_stream);
  for (uint32_t i = 0; i < covar_size; i++) {
    covariance[i] = covariance_stream.read();
  }

  data_t eigenvectors[MAX_BANDS*MAX_BANDS];
  data_t eigenvalues[MAX_BANDS*MAX_BANDS];
  svd_old(bands, covariance, eigenvectors, eigenvalues);
}
