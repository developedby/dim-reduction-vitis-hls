#include "types.hpp"
#include "center.hpp"
#include "array.hpp"
#include "xf_fintech/covariance.hpp"
#include <iostream>

constexpr int mem_unroll = 8;
constexpr int op_unroll = 8;

class MeanCenterBsqStream {
	large_data_t sum_band(uint32_t pixels, stream<data_t> &data) {
		large_data_t sum = 0;
		loop_sum_band:
		for (uint32_t px = 0; px < pixels; px++) {
			#pragma HLS loop_tripcount max = MAX_PIXELS min = MAX_PIXELS
			#pragma HLS unroll factor=op_unroll
			#pragma HLS expression_balance
			sum += data.read();
		}
		return sum;
	}
	
	void center_band(int32_t pixels, data_t mean, stream<data_t> &data, stream<data_t> &out) {
		loop_center_band:
		for (uint32_t px = 0; px < pixels; px++) {
			#pragma HLS loop_tripcount max = MAX_PIXELS min = MAX_PIXELS
			#pragma HLS unroll factor=op_unroll
			data_t val = data.read() - mean;
			out.write(val);
		}
	}

	public:

	MeanCenterBsqStream(uint32_t pixels, uint16_t bands, data_t* data) {
		const uint32_t pts = pixels * bands;
		const large_data_t pixels_data = pixels;
		stream<data_t> data1;
		stream<data_t> data2;
		stream<data_t> out;
		mem_to_stream<mem_unroll>(pts, data, data1);
		mem_to_stream<mem_unroll>(pts, data, data2);
		LOOP_MEAN_CENTER:
		for (uint16_t band = 0; band < bands; band++) {
			#pragma HLS loop_tripcount max = MAX_BANDS min = MAX_BANDS
			const large_data_t sum = sum_band(pixels, data1);
			const data_t mean = sum / pixels_data;
			center_band(pixels, mean, data2, out);
		}
		stream_to_mem<mem_unroll>(pts, out, data);
	}
};

class MeanCenterBipStream {
  const uint32_t pixels;
  const uint16_t bands;

  void sum_bands(large_data_t* means, stream<data_t> &read) {
    loop_sum_px:
    for (uint32_t px = 0; px < pixels; px++) {
			#pragma HLS loop_tripcount max = MAX_PIXELS min = MAX_PIXELS
      loop_sum_band:
      for (uint16_t band = 0; band < bands; band++) {
				#pragma HLS loop_tripcount max = MAX_BANDS min = MAX_BANDS
				#pragma HLS unroll factor=op_unroll
				#pragma HLS expression_balance
				const auto val = read.read();
        means[band] += val;
      }
    }
  }

  void div_means(large_data_t* means) {
		const large_data_t pixels_data = pixels;
    loop_div_mean:
    for (uint16_t band = 0; band < bands; band++) {
			#pragma HLS loop_tripcount max = MAX_BANDS min = MAX_BANDS
			#pragma HLS unroll factor=op_unroll
      means[band] /= pixels_data;
    }
  }

  void calculate_means(const uint32_t pts, const data_t* data, large_data_t* means) {
    stream<data_t> read;
    mem_to_stream<mem_unroll>(pts, data, read);
    sum_bands(means, read);
    div_means(means);
  }

  void subtract_mean(const large_data_t* means, stream<data_t> &read, stream<data_t> &write) {
    loop_center_px:
    for (uint32_t px = 0; px < pixels; px++) {
			#pragma HLS loop_tripcount max = MAX_PIXELS min = MAX_PIXELS
      loop_center_band:
      for (uint16_t band = 0; band < bands; band++) {
				#pragma HLS loop_tripcount max = MAX_BANDS min = MAX_BANDS
				#pragma HLS unroll factor=op_unroll
        const data_t val = read.read() - means[band];
        write.write(val);
      }
    }
  }

  void center(const uint32_t pts, data_t* data, const large_data_t* means) {
    #pragma HLS dataflow
    stream<data_t> read;
    stream<data_t> write;
    mem_to_stream<mem_unroll>(pts, data, read);
    subtract_mean(means, read, write);
    stream_to_mem<mem_unroll>(pts, write, data);
  }

public:
  MeanCenterBipStream(const uint32_t pixels, const uint16_t bands, data_t* data)
  : pixels(pixels), bands(bands) {
		const uint32_t pts = pixels*bands;
    large_data_t means[MAX_BANDS] = {0};
    calculate_means(pts, data, means);
    center(pts, data, means);
  }
};

class MeanCenterBsqBuf {
	data_t sum_band(const uint32_t pixels, const data_t* data) {
		const large_data_t pixels_data = 1 / large_data_t(pixels); // Multiplication is slightly cheaper
		data_t buf[READ_BUF_LEN];
		large_data_t sum = 0;
	
		LOOP_SUM_BAND:
		for (uint32_t i = 0; i < pixels; i += READ_BUF_LEN) {
			#pragma HLS loop_tripcount max = MAX_PIXEL_BUFS min = MAX_PIXEL_BUFS
			#pragma HLS pipeline off
			#pragma HLS dependence type=inter variable=data false
			#pragma HLS dependence type=intra variable=buf false
			const uint16_t read_len = (i+READ_BUF_LEN) <= pixels ? READ_BUF_LEN : (pixels % READ_BUF_LEN);
			// Read a constant size for faster copying
			copy_arr<mem_unroll>(READ_BUF_LEN, &data[i], &buf[0]);
			
			const large_data_t sum_buf = sum_arr<op_unroll>(read_len, &buf[0]);
			sum += sum_buf * pixels_data;
		}
		return sum;
	}

	void subtract_band(const uint32_t pixels, const uint32_t base_idx, const data_t mean, data_t* data) {
		data_t buf[READ_BUF_LEN];
		
		LOOP_SUBTRACT_BAND:
		for (uint32_t i = 0; i < pixels; i += READ_BUF_LEN) {
			#pragma HLS loop_tripcount max = MAX_PIXEL_BUFS min = MAX_PIXEL_BUFS
			#pragma HLS pipeline off
			#pragma HLS dependence type=inter variable=data false
			copy_arr<mem_unroll>(READ_BUF_LEN, &data[base_idx + i], &buf[0]);
			
			const uint16_t read_len = (i+READ_BUF_LEN) <= pixels ? READ_BUF_LEN : (pixels % READ_BUF_LEN);
			subtract_arr_num<op_unroll>(read_len, mean, &buf[0]);
			
			copy_arr<mem_unroll>(READ_BUF_LEN, &buf[0], &data[base_idx + i]);
		}
	}

	public:

	MeanCenterBsqBuf(const uint32_t pixels, const uint16_t bands, data_t* data) {
		const large_data_t pixels_data = 1 / large_data_t(pixels);
		for (uint16_t band = 0; band < bands; band++) {
			#pragma HLS loop_tripcount max = MAX_BANDS min = MAX_BANDS
			large_data_t sum = sum_band(pixels, &data[band*pixels]);
			//const data_t mean = sum / pixels_data;
			const data_t mean = sum;
			subtract_band(pixels, band*pixels, mean, data);
		}
	}
};

class MeanCenterBipBuf {
	void sum_bands(const uint32_t pixels, const uint16_t bands, const data_t* data, large_data_t* means) {
		const large_data_t pixels_data = 1 / large_data_t(pixels);
		data_t buf[MAX_BANDS];

		loop_sum_pxs:
		for (uint32_t px = 0; px < pixels; px++) {
			#pragma HLS loop_tripcount max = MAX_PIXELS min = MAX_PIXELS
			#pragma HLS pipeline off
			#pragma HLS dependence type=inter variable=data false
			copy_arr<mem_unroll>(bands, &data[px*bands], &buf[0]);
			loop_sum_bands:
			for (uint16_t band = 0; band < bands; band++) {
				#pragma HLS loop_tripcount max = MAX_BANDS min = MAX_BANDS
				#pragma HLS dependence type=inter false
				#pragma HLS unroll factor=op_unroll
				means[band] += buf[band] * pixels_data;
			}
		}
	}

	void div_means(const uint32_t pixels, const uint16_t bands, large_data_t* means) {
		const large_data_t pixels_data = 1 / large_data_t(pixels);

		loop_div_means:
		for (uint16_t band = 0; band < bands; band++) {
			#pragma HLS loop_tripcount max = MAX_BANDS min = MAX_BANDS
			#pragma HLS dependence type=inter false
			#pragma HLS unroll factor=op_unroll
			means[band] *= pixels_data;
		}
	}

	void center_bands(const uint32_t pixels, const uint16_t bands, const large_data_t* means, data_t* data) {
		data_t buf[MAX_BANDS];

		loop_sub_pxs:
		for (uint32_t px = 0; px < pixels; px++) {
			#pragma HLS loop_tripcount max = MAX_PIXELS min = MAX_PIXELS
			#pragma HLS dependence type=inter variable=data false
			copy_arr<mem_unroll>(bands, &data[px*bands], &buf[0]);
			loop_sub_bands:
			for (uint16_t band = 0; band < bands; band++) {
				#pragma HLS loop_tripcount max = MAX_BANDS min = MAX_BANDS
				#pragma HLS dependence type=inter false
				#pragma HLS unroll factor=op_unroll
				buf[band] -= data_t(means[band]);
			}
			copy_arr<mem_unroll>(bands, &buf[0], &data[px*bands]);
		}
	}

	public:

	MeanCenterBipBuf(const uint32_t pixels, const uint16_t bands, data_t* data) {
		large_data_t means[MAX_BANDS] = {0};

		sum_bands(pixels, bands, data, means);
		// Dividing each value instead of the sum gives smaller precision error.
		//div_means(pixels, bands, means);
		center_bands(pixels, bands, means, data);
	}
};

void mean_center_bsq_stream(const uint32_t pixels, const uint16_t bands, data_t* data) {
	#pragma HLS INTERFACE m_axi port=data bundle=gmem depth=MAX_POINTS
	MeanCenterBsqStream(pixels, bands, data);
}

void mean_center_bsq_buf(const uint32_t pixels, const uint16_t bands, data_t* data) {
	#pragma HLS INTERFACE m_axi port=data bundle=gmem depth=MAX_POINTS
	MeanCenterBsqBuf(pixels, bands, data);
}

void mean_center_bip_stream(const uint32_t pixels, const uint16_t bands, data_t* data) {
	#pragma HLS INTERFACE m_axi port=data bundle=gmem depth=MAX_POINTS
  MeanCenterBipStream(pixels, bands, data);
}

void mean_center_bip_buf(const uint32_t pixels, const uint16_t bands, data_t* data) {
	#pragma HLS INTERFACE m_axi port=data bundle=gmem depth=MAX_POINTS
  MeanCenterBipBuf(pixels, bands, data);
}

void mean_center_bip_mem(const uint32_t pixels, const uint16_t bands, data_t* data) {
	#pragma HLS INTERFACE m_axi port=data bundle=gmem depth=MAX_POINTS
  const large_data_t pixels_data = 1 / large_data_t(pixels);
	large_data_t means[MAX_BANDS] = {0};
	#pragma HLS bind_storage variable = means type = RAM_S2P
	
	for (uint32_t px = 0; px < pixels; px++) {
		#pragma HLS loop_tripcount max = MAX_PIXELS min = MAX_PIXELS
		for (uint16_t band = 0; band < bands; band++) {
			#pragma HLS loop_tripcount max = MAX_BANDS min = MAX_BANDS
			means[band] += data[px*bands + band] * pixels_data;
		}
	}

	for (uint32_t px = 0; px < pixels; px++) {
		#pragma HLS loop_tripcount max = MAX_PIXELS min = MAX_PIXELS
		for (uint16_t band = 0; band < bands; band++) {
			#pragma HLS loop_tripcount max = MAX_BANDS min = MAX_BANDS
			data[px*bands + band] = data[px*bands + band] - means[band];
		}
	}
}

void mean_center_bsq_mem(const uint32_t pixels, const uint16_t bands, data_t* data) {
	#pragma HLS INTERFACE m_axi port=data bundle=gmem depth=MAX_POINTS
  const large_data_t pixels_data = 1 / large_data_t(pixels);
	for (uint16_t band = 0; band < bands; band++) {
		#pragma HLS loop_tripcount max = MAX_BANDS min = MAX_BANDS
		large_data_t mean = 0;
		for (uint32_t px = 0; px < pixels; px++) {
			#pragma HLS loop_tripcount max = MAX_PIXELS min = MAX_PIXELS
			mean += data[band*pixels + px] * pixels_data;
		}
		for (uint32_t px = 0; px < pixels; px++) {
			#pragma HLS loop_tripcount max = MAX_PIXELS min = MAX_PIXELS
			data[band*pixels + px] = data[band*pixels + px] - mean;
		}
	}
}

void lib_mean_center_bsq(const int pixels, const int bands, data_t data[MAX_BANDS][MAX_PIXELS]) {
	#pragma HLS INTERFACE m_axi port=data bundle=gmem depth=MAX_POINTS
	xf::fintech::internal::aveImpl<data_t, MAX_BANDS, MAX_PIXELS, 2, 2, 16>(bands, pixels, data);
}


/* For comparing synthesis */

void mean_center_bsq_buf_ip(data_t* data) {
	#pragma HLS INTERFACE m_axi port=data bundle=gmem depth=MAX_POINTS
	mean_center_bsq_buf(MAX_PIXELS, MAX_BANDS, data);
}

void mean_center_bip_buf_ip(data_t* data) {
	#pragma HLS INTERFACE m_axi port=data bundle=gmem depth=MAX_POINTS
	mean_center_bip_buf(MAX_PIXELS, MAX_BANDS, data);
}

void mean_center_bsq_stream_ip(data_t* data) {
	#pragma HLS INTERFACE m_axi port=data bundle=gmem depth=MAX_POINTS
	mean_center_bsq_stream(MAX_PIXELS, MAX_BANDS, data);
}

void mean_center_bip_stream_ip(data_t* data) {
	#pragma HLS INTERFACE m_axi port=data bundle=gmem depth=MAX_POINTS
	mean_center_bip_stream(MAX_PIXELS, MAX_BANDS, data);
}

void mean_center_bip_mem_ip(data_t* data) {
	#pragma HLS INTERFACE m_axi port=data bundle=gmem depth=MAX_POINTS
  mean_center_bip_mem(MAX_PIXELS, MAX_BANDS, data);
}

void mean_center_bsq_mem_ip(data_t* data) {
	#pragma HLS INTERFACE m_axi port=data bundle=gmem depth=MAX_POINTS
  mean_center_bsq_mem(MAX_PIXELS, MAX_BANDS, data);
}

// From Vitis_Libraries covariance.hpp::aveImpl
void lib_mean_center_bsq_ip(data_t data[MAX_BANDS][MAX_PIXELS]) {
	#pragma HLS INTERFACE m_axi port=data bundle=gmem depth=MAX_POINTS
	lib_mean_center_bsq(MAX_PIXELS, MAX_BANDS, data);
}
