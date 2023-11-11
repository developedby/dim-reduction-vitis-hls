#ifndef COVARIANCE_HPP_
#define COVARIANCE_HPP_

#include "types.hpp"

void covariance_bsp(int pixels, int bands, hls::stream<data_t> &data, data_t covar[MAX_BANDS][MAX_BANDS]);

#endif