#pragma once

#include "types.hpp"

void covariance_bsq(uint32_t pixels, uint16_t bands, stream<data_t> &data1, stream<data_t> &data2, stream<data_t> &covar);
void covariance_bip(const uint32_t pixels, const uint16_t bands, const data_t* data, data_t* covar);
