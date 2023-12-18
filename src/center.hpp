#pragma once

#include "types.hpp"

void mean_center_bip_stream(const uint32_t pixels, const uint16_t bands, data_t* data);
void mean_center_bsq_stream(const uint32_t pixels, const uint16_t bands, data_t* data);
void mean_center_bip_buf(const uint32_t pixels, const uint16_t bands, data_t* data);
void mean_center_bsq_buf(const uint32_t pixels, const uint16_t bands, data_t* data);
void mean_center_bip_mem(const uint32_t pixels, const uint16_t bands, data_t* data);
void mean_center_bsq_mem(const uint32_t pixels, const uint16_t bands, data_t* data);

void lib_mean_center_bsq(const int pixels, const int bands, data_t data[MAX_BANDS][MAX_PIXELS]);
