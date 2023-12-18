#pragma once

#include "ap_fixed.h"
#include "hls_vector.h"
#include "hls_stream.h"

#include <cstdint>
#include <type_traits>

using hls::stream;

constexpr uint16_t MAX_COLS = 684;
constexpr uint16_t MAX_ROWS = 956;
constexpr uint16_t MAX_BANDS = 120;

/* constexpr uint16_t MAX_COLS = 8;
constexpr uint16_t MAX_ROWS = 8;
constexpr uint16_t MAX_BANDS = 64; */

constexpr uint32_t MAX_PIXELS = uint32_t(MAX_ROWS) * uint32_t(MAX_COLS);
constexpr uint32_t MAX_POINTS = MAX_PIXELS * uint32_t(MAX_BANDS);

constexpr bool use_float = true;

// The datatype used for most operations
typedef std::conditional<use_float, float, ap_fixed<32, 24>>::type data_t;
// The datatype used for operations that may hold a large number.
typedef std::conditional<use_float, float, ap_fixed<64, 48>>::type large_data_t;

// Size constant for read buffers
constexpr uint16_t READ_BUF_WIDTH_BYTES = 256;
constexpr uint16_t READ_BUF_LEN = READ_BUF_WIDTH_BYTES / sizeof(data_t);
constexpr uint16_t READ_BUF_LARGE_LEN = READ_BUF_WIDTH_BYTES / sizeof(large_data_t);

constexpr uint16_t MAX_PIXEL_BUFS = MAX_PIXELS%READ_BUF_LEN == 0 ? (MAX_PIXELS / READ_BUF_LEN) : (MAX_PIXELS / READ_BUF_LEN)+1;
