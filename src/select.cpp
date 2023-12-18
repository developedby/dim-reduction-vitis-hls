#include "types.hpp"
#include "select.hpp"

// Given a the eigenvector matrix, take only the first n vectors, efectivelly dropping some columns.
// This will shift the whole matrix.
void select_n_components(const uint16_t bands, const uint16_t n_components, data_t* components) {
  for (uint16_t band = 0; band < bands; band++) {
    for (uint16_t col = 0; col < n_components; col++) {
      components[band*n_components + col] = components[band*bands + col];
    }
  }
}