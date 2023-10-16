#ifndef TYPES_HPP_
#define TYPES_HPP_

#include "ap_fixed.h"
#include "hls_vector.h"
#include "hls_stream.h"

#include <cstdint>

typedef uint32_t dimsize_t;

constexpr dimsize_t MAX_COLS = 684;
constexpr dimsize_t MAX_ROWS = 956;
constexpr dimsize_t MAX_PIXELS = MAX_ROWS * MAX_COLS;
constexpr dimsize_t MAX_BANDS = 120;
constexpr dimsize_t MAX_POINTS = MAX_PIXELS * MAX_BANDS;


typedef float data_t;
typedef float large_data_t;

#endif
