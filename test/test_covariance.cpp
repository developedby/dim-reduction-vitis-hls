#include "center.hpp"
#include "covariance.hpp"
#include "types.hpp"
#include "xf_fintech/covariance.hpp"

#include <chrono>
#include <cmath>
#include <iostream>
#include <random>

enum Order {
	Bip, Bsq
};

enum Variant {
	Stream, Mem, Buf
};


void make_bsq_data(data_t* data) {
	if (use_random) {
		std::random_device rd;
    std::mt19937 gen(rd());
    std::exponential_distribution<data_t> d(1/expected_mean);
		for (uint32_t i = 0; i < points; i++) {
			data[i] = d(gen);
		}
	} else {
		for (uint32_t i = 0; i < points; i++) {
			data[i] = i % uint32_t(expected_mean*2);
		}
	}
}

void make_bip_data(data_t* data) {
	if (use_random) {
		std::random_device rd;
    std::mt19937 gen(rd());
    std::exponential_distribution<data_t> d(1/expected_mean);
		for (uint32_t i = 0; i < points; i++) {
			data[i] = d(gen);
		}
	} else {
		for (uint32_t i = 0; i < points; i++) {
			// Same data as bsq test, but here its transposed
			const auto idx = (i%pixels)*bands + (i/pixels);
			data[idx] = i % uint32_t(expected_mean*2);
		}
	}
}

void test_covariance_bip(const Variant variant) {
	std::cout << center_function_name(Order::Bip, variant) << std::endl;

	#ifdef __SYNTHESIS__
	data_t data[MAX_POINTS];
	#else
	std::vector<data_t> data(MAX_POINTS);
	#endif
	#pragma HLS INTERFACE m_axi port=data bundle=gmem depth=MAX_POINTS

	make_bip_data(&data[0]);

	auto start_time = std::chrono::high_resolution_clock::now();
	center_function(Order::Bip, variant, pixels, bands, &data[0]);
	auto end_time = std::chrono::high_resolution_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

	check_mean_err(&data[0], duration);
}

void test_covariance_bsq(const Variant variant) {
	std::cout << center_function_name(Order::Bsq, variant) << std::endl;

	#ifdef __SYNTHESIS__
	data_t data[MAX_POINTS];
	#else
	std::vector<data_t> data(MAX_POINTS);
	#endif
	#pragma HLS INTERFACE m_axi port=data bundle=gmem depth=MAX_POINTS

	make_bsq_data(&data[0]);

	auto start_time = std::chrono::high_resolution_clock::now();
	center_function(Order::Bsq, variant, pixels, bands, &data[0]);
	auto end_time = std::chrono::high_resolution_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

	check_mean_err(&data[0], duration);
}

void test_covariance_lib() {
	std::cout << "Mean centering lib\n";

	#ifdef __SYNTHESIS__
	data_t data[MAX_BANDS][MAX_PIXELS];
	#else
	std::vector<data_t> data_vec(MAX_POINTS);
	data_t (*data)[MAX_PIXELS] = reinterpret_cast<float (*)[MAX_PIXELS]>(&data_vec[0]);
	#endif
	#pragma HLS INTERFACE m_axi port=data bundle=gmem depth=MAX_POINTS

	make_bsq_data(&data[0][0]);

	auto start_time = std::chrono::high_resolution_clock::now();
	lib_mean_center_bsq(pixels, bands, data);
	auto end_time = std::chrono::high_resolution_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

	check_mean_err(&data[0][0], duration);
}


static int test_covariance(void)
{
	std::cout << "\n===== covariance test =====\n" << std::endl;

	const int test_matrix_bands = 3; // suggestion: more bands
	const int test_matrix_order = 3;

	/* 3x3x3 cube */
	data_t test_matrix[] =
	{
			64,  66,  68,
			69,  73,  80,
			71,  75,  73,

			580, 570, 590,
			660, 600, 630,
			570, 625, 665,

			29,  33,  37,
			46,  55,  25,
			43,  34,  22,
	};

	const data_t expected_result[] =
	{
			24.0,     87.5,    -7.0,
			87.5,   1343.75,  -85.0,
			-7.0,    -85.0,   111.25,
	};

	data_t result[NUM_ELEMS(expected_result)] = {};

	center_sequential(test_matrix_order, test_matrix_order, test_matrix_bands, test_matrix);
	covariance(test_matrix_order, test_matrix_order, test_matrix_bands, test_matrix, result);

	data_t diff = cumulative_percentile_error(NUM_ELEMS(expected_result), result, expected_result);

	if (diff <= 1.0)
	{
		std::cout << "SUCCESS.\n" << std::endl;
		return 0;
	}
	else
	{
		std::cout << "FAILED: Result did not match expected values.\n"
			   "error was " << diff << "%.\n" << std::endl;
		return 1;
	}
}

int main() {
	test_mean_centering_bip(Variant::Mem);
	test_mean_centering_bip(Variant::Buf);
	test_mean_centering_bsq(Variant::Mem);
	test_mean_centering_bsq(Variant::Buf);
	test_mean_centering_lib();
	return 0;
}