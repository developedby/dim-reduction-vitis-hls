#ifndef CENTERING_HPP_
#define CENTERING_HPP_

#include "types.hpp"

void mean_centering_bip(uint32_t pixels, uint16_t bands, stream<data_t> &data1, stream<data_t> &data2, stream<data_t> &out);
void mean_centering_bsq(uint32_t pixels, uint16_t bands, stream<data_t> &data1, stream<data_t> &data2, stream<data_t> &out);

void mean_centering_bip_to_bsq(uint32_t pixels, stream<Pixel<data_t>> &data1, stream<Pixel<data_t>> &data2, stream<Pixel<data_t>> &out);
void mean_centering_bsq_par(
    uint32_t pixels,
    stream<data_t> (&data1)[MAX_BANDS],
    stream<data_t> (&data2)[MAX_BANDS],
    stream<data_t> (&out)[MAX_BANDS]);

#endif
