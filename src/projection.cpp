
#include "projection.hpp"

// Does matrix multiplication
void projection_bip(
	const uint32_t pixels,
	const uint16_t bands,
	const uint16_t n_components,
	const data_t* components,
	const data_t* centered_data,
	data_t* transformed_data
) {
	LOOP_PROJECTION_PIXEL:
	for (uint32_t pixel = 0; pixel < pixels; pixel++) {
		LOOP_PROJECTION_COMPONENT:
		for (uint16_t component = 0; component < n_components; component++) {
			data_t val = 0;
			for (uint16_t band = 0; band < bands; band++) {
				val += centered_data[pixel*bands + band] * components[band*n_components + component];
			}
			transformed_data[pixel*n_components + component] = val;
		}
	}
}

// Does matrix multiplication, but BSQ is the transpose. So actually does Component^T * data
void projection_bsq(
	const uint32_t pixels,
	const uint16_t bands,
	const uint16_t n_components,
	const data_t* components,
	const data_t* centered_data,
	data_t* transformed_data
) {
	for (uint32_t i = 0; i < pixels*n_components; i++) {
		transformed_data[i] = 0;
	}

	for (uint16_t band = 0; band < bands; band++) {
		for (uint16_t component = 0; component < n_components; component++) {
			data_t component_val = components[band*n_components + component];
			for (uint32_t pixel = 0; pixel < pixels; pixel++)	{
				transformed_data[component*pixels + pixel] += component_val * centered_data[band*pixels + pixel];
			}
		}
	}
}


void projection_bip_ip(const data_t* components, const data_t* centered_data, data_t* transformed_data) {
	projection_bip(MAX_PIXELS, MAX_BANDS, 10, components, centered_data, transformed_data);
}
