#include "types.hpp"
#include "centering.hpp"
#include "transpose.hpp"

// TODO: What if instead of 2 streams, we tried an array specified as axis? I wonder if the tool could infer

static data_t sum_band(uint32_t pixels, stream<data_t> &data) {
	data_t sum = 0;
	for (uint32_t px = 0; px < pixels; px += 2) {
		#pragma HLS pipeline II=2
		const data_t v1 = data.read();
		const data_t v2 = data.read();
		sum += v1 + v2;
	}
	return sum;
}

static void center_band(int32_t pixels, int16_t mean, stream<data_t> &data, stream<data_t> &out) {
	for (uint32_t px = 0; px < pixels; px++) {
		data_t val = data.read() - mean;
		out.write(val);
	}
}

static void mean_center_band(uint32_t pixels, data_t pixels_data, stream<data_t> &data1, stream<data_t> &data2, stream<data_t> &out) {
	data_t sum = sum_band(pixels, data1);
	data_t mean = sum / pixels_data;
	center_band(pixels, mean, data2, out);
}

void mean_centering_bsq(uint32_t pixels, uint16_t bands, stream<data_t> &data1, stream<data_t> &data2, stream<data_t> &out) {
	// TODO: output the bands in parallel
	data_t pixels_data = pixels;
	for (uint16_t band = 0; band < bands; band++) {
		mean_center_band(pixels, pixels_data, data1, data2, out);
	}
}

void mean_centering_bsq_par(
	uint32_t pixels,
	stream<data_t> (&data1)[MAX_BANDS],
	stream<data_t> (&data2)[MAX_BANDS],
	stream<data_t> (&out)[MAX_BANDS]) {
	#pragma HLS dataflow
	data_t pixels_data = pixels;
	for (uint16_t band = 0; band < MAX_BANDS; band++) {
		#pragma HLS unroll
		mean_center_band(pixels, pixels_data, data1[band], data2[band], out[band]);
	}
}

template <typename T>
static void bip_to_bsq(uint32_t pixels, stream<Pixel<T>> &bip, stream<T> (&bsq)[MAX_BANDS]) {
	for(uint32_t px = 0; px < pixels; px++) {
		#pragma HLS dataflow
		Pixel<T> pixel = bip.read();
		for (uint16_t band = 0; band < MAX_BANDS; band++) {
			#pragma HLS pipeline
			bsq[band].write(pixel.data[band]);
		}
	}
}

template <typename T>
static void bsq_to_bip(uint32_t pixels, stream<T> (&bsq)[MAX_BANDS], stream<Pixel<T>> &bip) {
	for(uint32_t px = 0; px < pixels; px++) {
		Pixel<T> out_px;
		for (uint16_t band = 0; band < MAX_BANDS; band++) {
			out_px.data[band] = bsq[band].read();
		}
		bip.write(out_px);
	}
}

void mean_centering_bip_to_bsq(uint32_t pixels, stream<Pixel<data_t>> &data1, stream<Pixel<data_t>> &data2, stream<Pixel<data_t>> &out) {
	#pragma HLS dataflow

	stream<data_t> data1_bands[MAX_BANDS];
	stream<data_t> data2_bands[MAX_BANDS];
	stream<data_t> out_bands[MAX_BANDS];
	#pragma HLS array_partition variable=data1_bands dim=0
	#pragma HLS array_partition variable=data2_bands dim=0
	#pragma HLS array_partition variable=out_bands dim=0

	bip_to_bsq(pixels, data1, data1_bands);
	bip_to_bsq(pixels, data2, data2_bands);
	mean_centering_bsq_par(pixels, data1_bands, data2_bands, out_bands);
	bsq_to_bip(pixels, out_bands, out);
}

void mean_centering_bip(uint32_t pixels, uint16_t bands, stream<data_t> &data1, stream<data_t> &data2, stream<data_t> &out) {
	data_t means[MAX_BANDS] = {0};
	
	for (uint32_t px = 0; px < pixels; px++) {
		for (uint16_t band = 0; band < bands; band++) {
			means[band] += data1.read();
		}
	}

	for (uint16_t band = 0; band < MAX_BANDS; band++) {
		means[band] /= pixels;
	}

	for (uint32_t px = 0; px < pixels; px++) {
		for (uint16_t band = 0; band < bands; band++) {
			const data_t val = data2.read() - means[band];
			out.write(val);
		}
	}
}

void mean_centering_bip_to_bsq_ip(stream<Pixel<data_t>> &data1, stream<Pixel<data_t>> &data2, stream<Pixel<data_t>> &out) {
	mean_centering_bip_to_bsq(MAX_PIXELS, data1, data2, out);
}

void mean_centering_bsq_ip(stream<data_t> &data1, stream<data_t> &data2, stream<data_t> &out) {
	mean_centering_bsq(MAX_PIXELS, MAX_BANDS, data1, data2, out);
}

void mean_centering_bip_ip(stream<data_t> &data1, stream<data_t> &data2, stream<data_t> &out) {
	mean_centering_bip(MAX_PIXELS, MAX_BANDS, data1, data2, out);
}
