#pragma once

#include "types.hpp"

void pca_bip(
  const uint32_t pixels,
  const uint16_t bands,
  const uint32_t covariance_sampling,
  const uint16_t n_components,
  data_t *data_in,
  data_t *data_out
);
