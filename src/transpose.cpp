#include "types.hpp"
#include "transpose.hpp"

// Transposes a rows x cols matrix into a cols x rows matrix
template <typename T, typename ROW_T, typename COL_T>
void transpose_mat(const ROW_T rows, const COL_T cols, T* in, T* out) {
	for (COL_T col = 0; col < cols; col++) {
		for (ROW_T row = 0; row < rows; row++) {
			out[col*rows + row] = in[row*cols + col];
		}
	}
}

template <typename T>
void bsq_to_bip(const uint32_t pixels, const uint16_t bands, stream<T> &bsq, stream<T> &bip) {
  hls::stream<data_t> split_bands[MAX_BANDS];

	LOOP_TRANSPOSE_STREAM_IN_D:
	for (int band = 0; band < MAX_BANDS; band++)	{
		LOOP_TRANSPOSE_STREAM_IN_N:
		for (int px = 0; px < MAX_PIXELS; px++) {
			if (band < bands && px < pixels) {
				const T x = bsq.read();
				split_bands[band].write(x);
			}
		}
	}

	LOOP_TRANSPOSE_STREAM_OUT_N:
	for (int px = 0; px < MAX_PIXELS; px++) {
		LOOP_TRANSPOSE_STREAM_OUT_D:
		for (int band = 0; band < MAX_BANDS; band++) {
			if (band < bands && px < pixels) {
				const T x = split_bands[band].read();
				bip.write(x);
			}
		}
	}
}

void bip_to_bsq() {

}
