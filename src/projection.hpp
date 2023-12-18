#pragma once

#include "types.hpp"

void projection_bip(
	const uint32_t pixels,
	const uint16_t bands,
	const uint16_t n_components,
	const data_t* components,
	const data_t* centered_data,
	data_t* transformed_data
);

void projection_bsq(
	const uint32_t pixels,
	const uint16_t bands,
	const uint16_t n_components,
	const data_t* components,
	const data_t* centered_data,
	data_t* transformed_data
);
