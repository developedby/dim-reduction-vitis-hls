// Adapted from "Efficient Hardware for Principal Component Analysis of Hyperspectral Images", Olivier Lesage, 2023
#include "jacobi.hpp"
#include "hls_math.h"

constexpr uint8_t MAX_ITERATIONS = 10;

static void sort_eigen(int n, data_t vectors[MAX_BANDS*MAX_BANDS], data_t values[MAX_BANDS]) {
	/* todo use vitis library sort for more efficient hardware */

	LOOP_SORT_EIGEN: for ( int k = 0; k < MAX_BANDS - 1; k++ ) {
		if (k < n - 1) {
			int m = k;

			for (int l = 0; l < MAX_BANDS; l++) {
				if ( (l >= k + 1) && (l < n) ) {
					if (values[l] > values[m]) {
						m = l;
					}
				}
			}

			if (m != k) {
				data_t t = values[m];
				values[m] = values[k];
				values[k] = t;

				for (int i = 0; i < MAX_BANDS; i++) {
					if (i < n) {
						data_t w = vectors[i+m*n];
						vectors[i+m*n] = vectors[i+k*n];
						vectors[i+k*n] = w;
					}
				}
			}
		}
	}
}


static void diag_of ( int n, data_t a[MAX_BANDS*MAX_BANDS], data_t v[MAX_BANDS] )
{
	LOOP_GET_DIAG: for ( int i = 0; i < MAX_BANDS; i++ )
	{
		if (i < n)
		{
			v[i] = a[i+i*n];
		}
	}
}

static void id_matrix  ( int n, data_t a[MAX_BANDS*MAX_BANDS] )
{
  int k = 0;

  LOOP_BUILD_ID_MATRIX_i: for ( int j = 0; j < MAX_BANDS; j++ )
  {
	  LOOP_BUILD_ID_MATRIX_j: for ( int i = 0; i < MAX_BANDS; i++ )
	  {
		  if (j < n && i < n)
		  {
			  if ( i == j )
			  {
				  a[k] = 1.0;
			  }
			  else
			  {
				  a[k] = 0.0;
			  }

			  k++;
		  }
	  }
  }
}

void jacobi_eigenvalue ( int n, data_t a[MAX_BANDS*MAX_BANDS], data_t v[MAX_BANDS*MAX_BANDS], data_t d[MAX_BANDS]) {
	data_t c;
	data_t g;
	data_t gapq;
	data_t h;
	data_t s;
	data_t t;
	data_t tau;
	data_t term;
	data_t termp;
	data_t termq;
	data_t w;

	const data_t one = 1.0;
	const data_t ten = 10.0;
	const data_t four = 4.0;
	const data_t zero = 0.0;
	const data_t point_five = 0.5;

	id_matrix( n, v );
	diag_of( n, a, d );

	data_t bw[MAX_BANDS];
	data_t zw[MAX_BANDS] = {};

	LOOP_JACOBI_PRE: for (int i = 0; i < MAX_BANDS; i++ ) {
		if (i < n) {
			bw[i] = d[i];
		}
	}

	LOOP_JACOBI: for (int iter = 0; iter < MAX_ITERATIONS; iter++) {
		data_t convergence_threshold = zero;

		LOOP_CONVERGENCE_THRESHOLD: for ( int j = 0; j < MAX_BANDS; j++ ) {
			for ( int i = 0; i < MAX_BANDS; i++ ) {
				if (j < n && i < j) {
					convergence_threshold += a[i + j * n] * a[i + j * n];
				}
			}
		}

		convergence_threshold = hls::sqrt ( convergence_threshold ) / ( four * n );

		if ( convergence_threshold == zero ) {
			break;
		}

		LOOP_JACOBI_MAIN_p: for ( int p = 0; p < MAX_BANDS; p++ ) {
			if (p < n) {
				LOOP_JACOBI_MAIN_q: for ( int q = 0; q < MAX_BANDS; q++ ) {
					if ((q >= p + 1) && (q < n)) {
						gapq = ten * hls::fabs ( a[p+q*n] );
						termp = gapq + hls::fabs ( d[p] );
						termq = gapq + hls::fabs ( d[q] );

						if (4 < iter && termp == hls::fabs(d[p]) && termq == hls::fabs(d[q])) {
							a[p+q*n] = zero;
						} else {
							h = d[q] - d[p];
							term = hls::fabs ( h ) + gapq;

							if (term == hls::fabs(h)) {
								t = a[p+q*n] / h;
							} else {
								data_t theta = point_five * h / a[p+q*n];
								t = one / ( hls::fabs ( theta ) + hls::sqrt ( one + theta * theta ) );
								if (theta < zero) {
									t = -t;
								}
							}
							c = one / hls::sqrt ( one + t * t );
							s = t * c;
							tau = s / ( one + c );
							h = t * a[p+q*n];

							zw[p] -= h;
							zw[q] += h;
							d[p] -= h;
							d[q] += h;

							a[p+q*n] = 0;

							LOOP_ROT_1:
							for (int j = 0; j < MAX_BANDS; j++) {
								if (j < p) {
									g = a[j+p*n];
									h = a[j+q*n];
									a[j+p*n] = g - s * (h + g * tau);
									a[j+q*n] = h + s * (g - h * tau);
								}
							}

							LOOP_ROT_2:
							for (int j = 0; j < MAX_BANDS; j++) {
								if ((j >= p + 1) && (j < q)) {
									g = a[p+j*n];
									h = a[j+q*n];
									a[p+j*n] = g - s * ( h + g * tau );
									a[j+q*n] = h + s * ( g - h * tau );
								}
							}

							LOOP_ROT_3:
							for (int j = 0; j < MAX_BANDS; j++) {
								if ((j >= q + 1) && (j < n)) {
									g = a[p+j*n];
									h = a[q+j*n];
									a[p+j*n] = g - s * ( h + g * tau );
									a[q+j*n] = h + s * ( g - h * tau );
								}
							}

							LOOP_UPDATE_VECTORS:
							for (int j = 0; j < MAX_BANDS; j++ ) {
								if (j < n) {
									g = v[j+p*n];
									h = v[j+q*n];
									v[j+p*n] = g - s * ( h + g * tau );
									v[j+q*n] = h + s * ( g - h * tau );
								}
							}
						}
					}
				}
			}
		}

		LOOP_POST_OP:
		for (int i = 0; i < MAX_BANDS; i++ ) {
			#pragma HLS PIPELINE
			if (i < n) {
				bw[i] += zw[i];
				d[i] = bw[i];
				zw[i] = zero;
			}
		}
	}

	sort_eigen(n, v, d);
}
