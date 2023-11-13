
#include "types.hpp"
#include "hls_stream.h"

/* This algorithm is applied the (square) covariance matrix,
 * which has dimensions [MAX_DIM x MAX_DIM]
 */

void jacobi_eigenvalue(int bands,
		data_t matrix[MAX_BANDS*MAX_BANDS],
		data_t eigenvectors[MAX_BANDS*MAX_BANDS],
		data_t eigenvalues[MAX_BANDS]);

void jacobi_eigenvalue( int n,
		hls::stream<data_t> &matrix,
		hls::stream<data_t> &eigenvectors,
		hls::stream<data_t> &eigenvalues);

/* Wrapper for jacobi_eigenvalue(). Due to issues with Vitis synthesizing overloaded functions */
void jacobi_eigenvalue_ip( int n,
		hls::stream<data_t> &matrix,
		hls::stream<data_t> &eigenvectors,
		hls::stream<data_t> &eigenvalues);

