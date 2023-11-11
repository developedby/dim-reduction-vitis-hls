#include "types.hpp"
#include "centering.hpp"

static void sum_bands_bip(dim_t bands, dim_t pixels, stream<data_t> &data, large_data_t means[MAX_BANDS]);
static void div_mean_bip (data_t pixels, data_t means[MAX_BANDS]);
static void center_pixels_bip(dim_t bands, dim_t pixels, stream<data_t> &data, stream<data_t> &out, data_t means[MAX_BANDS]);

// Subtracts the mean from each band of the data.
// Data in band-interleaved order.
// Reads the memory twice, through 2 separate streams.
void mean_centering_bip(dim_t bands, dim_t pixels, stream<data_t> &data1, stream<data_t> &data2, stream<data_t> &out) {
	// To help inferring burst accesses
	assert(pixels % 16 == 0);
	assert(bands % 8 == 0);

	const data_t pixels_data = pixels;
	large_data_t means[MAX_BANDS] = {0};

	sum_bands_bip(bands, pixels, data1, means);
	div_mean_bip(pixels_data, means);
	center_pixels_bip(bands, pixels, data2, out, means);
}

void sum_bands_bip(dim_t bands, dim_t pixels, stream<data_t> &data, large_data_t means[MAX_BANDS]) {
	loop_sum_pixels:
	for (dim_t px = 0; px < pixels; px++) {
		loop_sum_bands:
		for (dim_t band = 0; band < bands; band++) {
			means[band] += data.read();
		}
	}
}

void div_mean_bip (data_t pixels, data_t means[MAX_BANDS]) {
	// We don't really mind dividing unused elements of the array since they're 0 anyway.
	loop_mean_div:
	for (dim_t band = 0; band < MAX_BANDS; band++) {
		means[band] /= pixels;
	}
}

void center_pixels_bip(dim_t bands, dim_t pixels, stream<data_t> &data, stream<data_t> &out, large_data_t means[MAX_BANDS]) {
	loop_center_pixels:
	for (dim_t px = 0; px < pixels; px++) {
		loop_center_bands:
		for (dim_t band = 0; band < bands; band++) {

				out.write(data.read() - means[band]);

		}
	}
}

// Subtracts the mean from each band of the data.
// Data in band-sequential order.
void mean_centering_bsp(dim_t bands, dim_t pixels, data_t data[MAX_BANDS][MAX_PIXELS]) {
	data_t means[MAX_BANDS] = {0};
	loop_sum_bands:
	for (dim_t band = 0; band < MAX_BANDS; band++) {
		if (band < bands) {
			loop_sum_pxs:
			for (dim_t px = 0; px < MAX_PIXELS; px++) {
				if (px < pixels) {
					means[band] += data[band][px];
				}
			}
		}
	}
	loop_mean_div:
	for (dim_t band = 0; band < MAX_BANDS; band++) {
		if (band < bands) {
			means[band] /= pixels;
		}
	}
	loop_center_bands:
	for (dim_t band = 0; band < MAX_BANDS; band++) {
		if (band < bands) {
			loop_center_pxs:
			for (dim_t px = 0; px < MAX_PIXELS; px++) {
				if (px < pixels) {
					data[band][px] -= means[band];
				}
			}
		}
	}
}
