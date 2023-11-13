#include "centering.hpp"
#include "types.hpp"

#include <iostream>

static void make_data(int32_t const points, uint8_t * const data) {
	for (uint32_t i = 0; i < points; i++) {
		data[i] = uint8_t(i);
	}
}

template <typename T>
static void arr_to_stream(uint32_t const points,  T const * const arr, stream<T> &str) {
	for (uint32_t i = 0; i < points; i++) {
		str.write(arr[i]);
	}
}

static void test_mean_centering_bip() {
	std::cout << "Mean centering BIP\n";

	constexpr uint32_t pixels = 64;
	constexpr uint16_t bands = 64;
	constexpr uint32_t points = pixels * bands;

	uint8_t data[MAX_POINTS];
	make_data(points, data);

	stream<uint8_t> data1;
	stream<uint8_t> data2;
	arr_to_stream(points, data, data1);
	arr_to_stream(points, data, data2);

	stream<int16_t> centered;
	mean_centering_bip(bands, pixels, data1, data2, centered);

	data_t sums[MAX_BANDS] = {0};
	for (uint i = 0; i < pixels; i++) {
		for (uint j = 0; j < bands; j++) {
			sums[j] += centered.read();
		}
	}
	for (uint i = 0; i < bands; i++) {
		std::cout << "Sum " << i << " : " << sums[i] << std::endl;
	}
}

static void test_mean_centering_bsq() {
	std::cout << "Mean centering bsq\n";

	constexpr uint32_t pixels = 64;
	constexpr uint16_t bands = 64;
	constexpr uint32_t points = pixels * bands;

	uint8_t data[MAX_POINTS];
	make_data(points, data);

	stream<uint8_t> data1;
	stream<uint8_t> data2;
	arr_to_stream(points, data, data1);
	arr_to_stream(points, data, data2);

	stream<int16_t> centered;
	mean_centering_bsq(bands, pixels, data1, data2, centered);

	data_t sums[MAX_BANDS] = {0};
	for (uint i = 0; i < bands; i++) {
		for (uint j = 0; j < pixels; j++) {
			auto val = centered.read();
			sums[i] += val;
		}
	}
	for (uint i = 0; i < bands; i++) {
		std::cout << "Sum " << i << " : " << sums[i] << std::endl;
	}
}

int main() {
	test_mean_centering_bip();
	test_mean_centering_bsq();
	return 0;
}
