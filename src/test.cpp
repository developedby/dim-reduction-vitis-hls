#include "types.hpp"
#include "centering.hpp"
#include <iostream>
#include "covariance.hpp"

static void test_mean_centering_bip() {
	std::cout << "Mean centering BIP\n";

	constexpr dimsize_t pixels = 20;
	constexpr dimsize_t bands = 10;
	data_t nums[MAX_PIXELS][MAX_BANDS];
	for (uint i = 0; i < pixels; i++) {
		for (uint j = 0; j < bands; j++) {
			nums[i][j] = data_t(i);
		}
	}

	mean_centering_bip(bands, pixels, nums);

	data_t sums[MAX_BANDS] = {0};
	for (uint i = 0; i < pixels; i++) {
		for (uint j = 0; j < bands; j++) {
			sums[j] += nums[i][j];
		}
	}
	for (uint i = 0; i < bands; i++) {
		std::cout << "Sum " << i << " : " << sums[i] << std::endl;
	}
}

static void test_mean_centering_bsp() {
	std::cout << "Mean centering BSP\n";

	constexpr dimsize_t pixels = 20;
	constexpr dimsize_t bands = 10;
	data_t nums[MAX_BANDS][MAX_PIXELS];
	for (uint i = 0; i < bands; i++) {
		for (uint j = 0; j < pixels; j++) {
			nums[i][j] = data_t(j);
		}
	}

	mean_centering_bsp(bands, pixels, nums);

	data_t sums[MAX_BANDS] = {0};
	for (uint i = 0; i < bands; i++) {
		for (uint j = 0; j < pixels; j++) {
			sums[i] += nums[i][j];
		}
	}
	for (uint i = 0; i < bands; i++) {
		std::cout << "Sum " << i << " : " << sums[i] << std::endl;
	}
}

int main() {
	test_mean_centering_bip();
	test_mean_centering_bsp();
	return 0;
}
