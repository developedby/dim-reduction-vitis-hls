#include "types.hpp"
#include "centering.hpp"
#include <iostream>
#include "covariance.hpp"

/*
static void test_mean_centering_bip() {
	data_t * nums = new data_t[MAX_POINTS];
	hls::stream<data_t> input_stream;
	hls::stream<data_t> output_stream;

	for (dimsize_t i = 0; i < MAX_POINTS; i++) {
		nums[i] = data_t(i);
		input_stream.write(nums[i]);
	}

	mean_centering_bip<MAX_ROWS, MAX_COLS, MAX_BANDS>(input_stream, output_stream);

	large_data_t sum = 0;
	for (dimsize_t i = 0; i < MAX_POINTS; i++) {
		sum += output_stream.read();
	}

	std::cout << sum << std::endl;
}
*/

static void test_mean_centering_bsp() {
	constexpr dimsize_t pixels = 20;
	constexpr dimsize_t bands = 20;
	data_t nums[bands][pixels];
	for (uint i = 0; i < bands; i++) {
		for (uint j = 0; j < pixels; j++) {
			nums[i][j] = data_t(j);
		}
	}

	xf::fintech::internal::aveImpl<data_t, bands, pixels, 4, 1, 16>(bands, pixels, nums);

	for (uint i = 0; i < bands; i++) {
		data_t sum = 0;
		for (uint j = 0; j < pixels; j++) {
			sum += nums[i][j];
		}
		std::cout << "Sum " << i << " : " << sum << std::endl;
	}
}

int main() {
	// test_mean_centering_bip();
	test_mean_centering_bsp();
	return 0;
}
