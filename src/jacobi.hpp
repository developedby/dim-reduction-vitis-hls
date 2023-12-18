#pragma once

#include "types.hpp"
/* This algorithm is applied the (square) covariance matrix,
 * which has dimensions [MAX_DIM x MAX_DIM]
 */

void jacobi_eigenvalue(int bands,
		data_t matrix[MAX_BANDS*MAX_BANDS],
		data_t eigenvectors[MAX_BANDS*MAX_BANDS],
		data_t eigenvalues[MAX_BANDS]);
