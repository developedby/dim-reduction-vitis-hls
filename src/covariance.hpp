#ifndef COVARIANCE_HPP_
#define COVARIANCE_HPP_

#include "types.hpp"

void covariance_bsq(uint32_t pixels, uint16_t bands, stream<data_t> &data1, stream<data_t> &data2, stream<data_t> &covar);

#endif