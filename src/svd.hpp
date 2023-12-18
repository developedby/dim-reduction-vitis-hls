#pragma once

#include "types.hpp"

void svd_old(uint16_t bands, data_t covar[MAX_BANDS*MAX_BANDS], data_t eigenvectors[MAX_BANDS*MAX_BANDS], data_t eigenvalues[MAX_BANDS]);
