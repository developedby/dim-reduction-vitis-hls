#include "types.hpp"
#include "centering.hpp"
#include "covariance.hpp"

static void calculate_means_bip(dimsize_t bands, dimsize_t pixels, data_t data[MAX_BANDS][MAX_PIXELS], data_t means[MAX_BANDS]);
static void subtract_means_bip(dimsize_t bands, dimsize_t pixels, data_t data[MAX_BANDS][MAX_PIXELS], data_t means[MAX_BANDS]);

// TODO: Partition arrays into each separate band.

// Subtracts the mean from each band of the data.
// Data in BIP order.
void mean_centering_bip(dimsize_t bands, dimsize_t pixels, data_t data[MAX_BANDS][MAX_PIXELS]) {
	data_t means[MAX_BANDS] = {};
	calculate_means_bip(bands, pixels, data, means);
	subtract_means_bip(bands, pixels, data, means);
}

void mean_centering_bsp(dimsize_t bands, dimsize_t pixels, data_t data[MAX_BANDS][MAX_PIXELS]) {
	data_t means[MAX_BANDS] = {};

	loop_sum_bands:
	for (int i = 0; i < MAX_BANDS; i++) {
		if (i < bands) {
			loop_sum_pxs:
			for (int j = 0; j < MAX_PIXELS; j++) {
				if (j < num_elems) {
					means[i] += matrix[i][j];
				}
			}
		}
	}

	loop_div_means:
	for (int i = 0; i < MAX_BANDS; i++) {
		if (i < bands) {
			means[i] /= pixels;
		}
	}

	loop_center_bands:
	for (int i = 0; i < MAX_BANDS; i++) {
		if (i < bands) {
			loop_center_pxs:
			for (int j = 0; j < MAX_PIXELS; j++) {
				if (j < pixels) {
					data[i][j] -= means[i];
				}
			}
		}
	}
}

void calculate_means_bip(dimsize_t bands, dimsize_t pixels, data_t data[MAX_BANDS][MAX_PIXELS], data_t means[MAX_BANDS]) {
	loop_sum_pixels:
	for (int i = 0; i < MAX_PIXELS; i++) {
		if (i < pixels) {
			loop_sum_bands:
			for (dimsize_t j = 0; j < MAX_BANDS; j++) {
				if (j < bands) {
					means[i] += data[i][j];
				}
			}
		}
	}

	loop_mean_div:
	for (dimsize_t d = 0; d < BANDS; d++) {
		means[d] /= data_t(BANDS);
	}
}

void subtract_means_bip(dimsize_t bands, dimsize_t pixels, data_t data[MAX_BANDS][MAX_PIXELS], data_t means[MAX_BANDS]) {
	loop_pixels:
	for (dimsize_t i = 0; i < num_pixels; i++) {
		loop_bands:
		for (dimsize_t j = 0; j < BANDS; j ++) {
			data[i][j] -= means[i];
		}
	}
}