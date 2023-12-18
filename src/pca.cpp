#include "types.hpp"
#include "pca.hpp"
#include "center.hpp"
#include "covariance.hpp"
#include "svd.hpp"
#include "projection.hpp"
#include "select.hpp"
#include "xf_fintech/pca.hpp"

void pca_bsq(const uint32_t pixels, const uint16_t bands, data_t *data_in, data_t *data_tmp)
{
  const uint32_t points = pixels * bands;
  const uint32_t covar_size = bands * bands;

  mean_center_bsq_buf(pixels, bands, data_in);

  stream<data_t> centered1;
  for (uint32_t pt = 0; pt < points; pt++)
  {
    centered1.write(data_in[pt]);
  }
  stream<data_t> centered2;
  for (uint16_t band_lft = 0; band_lft < bands; band_lft++)
  {
    for (uint32_t pt = 0; pt < points; pt++)
    {
      centered2.write(data_in[pt]);
    }
  }

  data_t covariance[MAX_BANDS * MAX_BANDS];
  stream<data_t> covariance_stream;
  covariance_bsq(pixels, bands, centered1, centered2, covariance_stream);
  for (uint32_t i = 0; i < covar_size; i++)
  {
    covariance[i] = covariance_stream.read();
  }

  data_t eigenvectors[MAX_BANDS * MAX_BANDS];
  data_t eigenvalues[MAX_BANDS * MAX_BANDS];
  svd_old(bands, covariance, eigenvectors, eigenvalues);
}

void pca_bip(
    const uint32_t pixels,
    const uint16_t bands,
    const uint32_t covariance_sampling,
    const uint16_t n_components,
    data_t *data_in,
    data_t *data_out)
{
  mean_center_bip_buf (pixels, bands, data_in);

  data_t covariance[MAX_BANDS * MAX_BANDS];
  // TODO: Actual random sampling
  covariance_bip(covariance_sampling, bands, data_in, covariance);

  data_t eigenvectors[MAX_BANDS * MAX_BANDS];
  data_t eigenvalues[MAX_BANDS * MAX_BANDS];
  svd_old(bands, covariance, eigenvectors, eigenvalues);

  data_t *components = eigenvectors;
  select_n_components(bands, n_components, components);

  //projection_bip(pixels, bands, n_components, components, data_in, data_out);
}

template<unsigned int n_components, unsigned int parallel_units>
void pca_bsq_lib(const uint32_t pixels, const uint16_t bands, data_t data_in[MAX_BANDS][MAX_PIXELS], data_t data_out[MAX_BANDS][MAX_PIXELS]) {
  auto pca = xf::fintech::PCA<data_t, n_components, parallel_units, MAX_BANDS, MAX_PIXELS>(bands, pixels, data_in);
  data_t components[bands][n_components];
  pca.getComponents(components);
  projection_bsq(pixels, bands, n_components, &components[0][0], &data_in[0][0], &data_out[0][0]);
}

void pca_bip_ip(data_t *data_in, data_t *data_out)
{
  pca_bip(MAX_PIXELS, MAX_BANDS, MAX_BANDS * 10, 10, data_in, data_out);
}

void pca_bsq_lib_ip(data_t data_in[MAX_BANDS][MAX_PIXELS], data_t data_out[MAX_BANDS][MAX_PIXELS]) {
  pca_bsq_lib<10, 1>(MAX_PIXELS, MAX_BANDS, data_in, data_out);
}