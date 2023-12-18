#include "types.hpp"
#include "svd.hpp"
#include "jacobi.hpp"
//#include "hw/EigenSolver/syevj.hpp"

void svd_old(uint16_t bands, data_t covar[MAX_BANDS*MAX_BANDS], data_t eigenvectors[MAX_BANDS*MAX_BANDS], data_t eigenvalues[MAX_BANDS]) {
  jacobi_eigenvalue(bands, covar, eigenvectors, eigenvalues);
}

/* void svd_lib(const uint16_t bands, data_t covar[MAX_BANDS*MAX_BANDS], data_t eigenvectors[MAX_BANDS*MAX_BANDS], data_t eigenvalues[MAX_BANDS]) {
  int info; // unused
  xf::solver::syevj<data_t, MAX_BANDS, 4>(bands, standarisedData, noVars, eigVals, eigVecs, bands, info);
  xf::fintech::PCA<data_t, >()
} */