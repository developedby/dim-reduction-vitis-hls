#ifndef TYPES_HPP_
#define TYPES_HPP_

#include "ap_fixed.h"
#include "hls_vector.h"
#include "hls_stream.h"

#include <cstdint>
#include <type_traits>

using hls::stream;

// Minimum length for storing number of pixels, indexing in data cube, etc
typedef uint32_t dim_t;


/* constexpr dim_t MAX_COLS = 684;
constexpr dim_t MAX_ROWS = 956;
constexpr dim_t MAX_BANDS = 120; */

constexpr dim_t MAX_COLS = 16;
constexpr dim_t MAX_ROWS = 16;
constexpr dim_t MAX_BANDS = 16;

constexpr dim_t MAX_PIXELS = MAX_ROWS * MAX_COLS;
constexpr dim_t MAX_POINTS = MAX_PIXELS * MAX_BANDS;

constexpr bool use_float = true;

// The datatype used for most operations
typedef std::conditional<use_float, float, ap_fixed<32, 24>>::type data_t;
// The datatype used for operations that may hold a large number.
typedef std::conditional<use_float, float, ap_fixed<64, 48>>::type large_data_t;

#endif
