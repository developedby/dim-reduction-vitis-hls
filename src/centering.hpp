#ifndef CENTERING_HPP_
#define CENTERING_HPP_

#include "types.hpp"

void mean_centering_bip(dim_t bands, dim_t pixels, data_t data[MAX_PIXELS][MAX_BANDS]);
void mean_centering_bsp(dim_t bands, dim_t pixels, data_t data[MAX_BANDS][MAX_PIXELS]);

#endif
