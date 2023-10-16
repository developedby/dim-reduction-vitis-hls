#ifndef CENTERING_HPP_
#define CENTERING_HPP_

#include "types.hpp"

template <dimsize_t ROWS, dimsize_t COLS, dimsize_t BANDS>
void mean_centering_bip(hls::stream<data_t> &in, hls::stream<data_t> &out);

#endif
