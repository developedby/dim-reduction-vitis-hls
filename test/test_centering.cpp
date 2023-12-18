#include "center.hpp"
#include "types.hpp"
#include "xf_fintech/covariance.hpp"

#include <chrono>
#include <cmath>
#include <iostream>
#include <random>

constexpr uint32_t pixels = MAX_PIXELS;
constexpr uint16_t bands = MAX_BANDS;
constexpr uint32_t points = pixels * bands;
constexpr bool use_random = true;
constexpr data_t expected_mean = 128;
constexpr data_t err_threshold = 0.001; // 0.1%

enum Order {
	Bip, Bsq
};

enum Variant {
	Stream, Mem, Buf
};

void center_function(const Order order, const Variant variant, const uint32_t pixels, const uint16_t bands, data_t* data) {
	if (order == Order::Bip) {
		if (variant == Variant::Stream) {
			mean_center_bip_stream(pixels, bands, data);
		} else if (variant == Variant::Buf) {
			mean_center_bip_buf(pixels, bands, data);
		} else if (variant == Variant::Mem) {
			mean_center_bip_mem(pixels, bands, data);
		} else {
			__builtin_unreachable();
		}
	} else if (order == Order::Bsq) {
		if (variant == Variant::Stream) {
			mean_center_bsq_stream(pixels, bands, data);
		} else if (variant == Variant::Buf) {
			mean_center_bsq_buf(pixels, bands, data);
		} else if (variant == Variant::Mem) {
			mean_center_bsq_mem(pixels, bands, data);
		} else {
			__builtin_unreachable();
		}
	} else {
		__builtin_unreachable();
	}
}

std::string center_function_name(const Order order, const Variant variant) {
	if (order == Order::Bip) {
		if (variant == Variant::Stream) {
			return {"Mean Center BIP Stream"};
		} else if (variant == Variant::Buf) {
			return {"Mean Center BIP Buf"};
		} else if (variant == Variant::Mem) {
			return {"Mean Center BIP Mem"};
		} else {
			__builtin_unreachable();
		}
	} else if (order == Order::Bsq) {
		if (variant == Variant::Stream) {
			return {"Mean Center BSQ Stream"};
		} else if (variant == Variant::Buf) {
			return {"Mean Center BSQ Buf"};
		} else if (variant == Variant::Mem) {
			return {"Mean Center BSQ Mem"};
		} else {
			__builtin_unreachable();
		}
	} else {
		__builtin_unreachable();
	}
}

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

void check_mean_err(const data_t* data, const std::chrono::milliseconds duration) {
	large_data_t mean = 0;
	for (uint32_t i = 0; i < points; i++) {
		mean += data[i] / large_data_t(points);
	}
	const large_data_t mean_err = std::abs(mean) / expected_mean;
	if (mean_err < err_threshold) {
		std::cout << "OK. Took " << duration.count() << "ms." << " Mean err " << mean_err*100 << "% > " << err_threshold*100 << "%" << std::endl;
	} else {
		std::cout << "ERR. Mean err " << mean_err*100 << "% > " << err_threshold*100 << "%" << std::endl;
	}
}

void test_mean_centering_bip(const Variant variant) {
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

void test_mean_centering_bsq(const Variant variant) {
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

void test_mean_centering_lib() {
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

int main() {
	test_mean_centering_bip(Variant::Mem);
	test_mean_centering_bip(Variant::Buf);
	test_mean_centering_bsq(Variant::Mem);
	test_mean_centering_bsq(Variant::Buf);
	test_mean_centering_lib();
	return 0;
}
