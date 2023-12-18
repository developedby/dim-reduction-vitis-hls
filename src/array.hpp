#pragma once

#include "types.hpp"

// Copies data in and out of memory.
template <int unroll_factor>
void copy_arr(const uint16_t len, const data_t* src, data_t* dst) {
  assert(len % 8 == 0);
  LOOP_COPY_BUF:
  for (uint16_t i = 0; i < len; i++) {
    #pragma HLS loop_tripcount max=READ_BUF_LEN min=READ_BUF_LEN
    #pragma HLS pipeline
    #pragma HLS unroll factor=unroll_factor
    #pragma HLS dependence type=inter false
    dst[i] = src[i];
  }
}

// Streams from memory
template <int unroll_factor>
void mem_to_stream(const uint32_t len, const data_t* src, stream<data_t> &dst) {
  loop_mem_to_stream:
  for (uint32_t i = 0; i < len; i++) {
    #pragma HLS loop_tripcount max = MAX_POINTS min = MAX_POINTS
    #pragma HLS unroll factor=unroll_factor skip_exit_check
    #pragma HLS dependence type=inter variable=src false
    dst.write(src[i]);
  }
}

// Streams to memory
template <int unroll_factor>
void stream_to_mem(const uint32_t len, stream<data_t> &src, data_t* dst) {
  loop_stream_to_mem:for (uint32_t i = 0; i < len; i++) {
    #pragma HLS loop_tripcount max = MAX_POINTS min = MAX_POINTS
    #pragma HLS unroll factor=unroll_factor skip_exit_check
    #pragma HLS dependence type=inter variable=dst false
    dst[i] = src.read();
  }
}


template <typename t_DataType, unsigned int t_Entries, typename t_SumType = t_DataType>
class BinarySum {
  public:
  static const t_SumType sum(const t_DataType p_x[t_Entries]) {
    const unsigned int l_halfEntries = t_Entries >> 1;
    return BinarySum<t_DataType, l_halfEntries, t_SumType>::sum(p_x) +
           BinarySum<t_DataType, l_halfEntries, t_SumType>::sum(p_x + l_halfEntries);
  }
};
template <typename t_DataType, typename t_SumType>
class BinarySum<t_DataType, 1, t_SumType> {
  public:
  static const t_SumType sum(const t_DataType p_x[1]) {
    #pragma HLS INLINE
    return p_x[0];
  }
};


// Sums a local memory array. If DRAM is given, it'll be very slow.
template <int unroll_factor>
large_data_t sum_arr(const uint16_t len, const data_t* data) {
  large_data_t sum = 0;
  LOOP_SUM_ARR:
  for (uint16_t i = 0; i < len; i += unroll_factor) {
    #pragma HLS loop_tripcount max = READ_BUF_LEN/unroll_factor min = READ_BUF_LEN/unroll_factor
    #pragma HLS pipeline
    #pragma HLS dependence type=inter variable=data false
    sum += BinarySum<const data_t, unroll_factor, large_data_t>::sum(&data[i]);
  }
  return sum;
}

// Vector-scalar subraction. For local memory arrays. If DRAM is given, it'll be very slow.
template <int unroll_factor>
void subtract_arr_num(const uint16_t len, const data_t num, data_t* data) {
  LOOP_SUB_ARR:
  for (uint16_t i = 0; i < len; i++) {
    #pragma HLS loop_tripcount max = READ_BUF_LEN min = READ_BUF_LEN
    #pragma HLS unroll factor=unroll_factor skip_exit_check
    #pragma HLS dependence type=inter variable=data false
    data[i] -= num;
  }
}

template <typename t_DataType, unsigned int t_Entries, typename t_SumType = t_DataType>
class BinaryDotProduct {
  public:
  static const t_SumType sum(const t_DataType in1[t_Entries], const t_DataType in2[t_Entries]) {
    const unsigned int l_halfEntries = t_Entries >> 1;
    return BinaryDotProduct<t_DataType, l_halfEntries, t_SumType>::sum(in1, in2) +
           BinaryDotProduct<t_DataType, l_halfEntries, t_SumType>::sum(in1 + l_halfEntries, in2 + l_halfEntries);
  }
};
template <typename t_DataType, typename t_SumType>
class BinaryDotProduct<t_DataType, 1, t_SumType> {
  public:
  static const t_SumType sum(const t_DataType in1[1], const t_DataType in2[1]) {
    #pragma HLS INLINE
    return in1[0] * in2[0];
  }
};

// Sums a local memory array. If DRAM is given, it'll be very slow.
template <int unroll_factor>
large_data_t dot_product(const uint16_t len, const data_t* in1, const data_t* in2) {
  large_data_t sum = 0;
  LOOP_SUM_ARR:
  for (uint16_t i = 0; i < len; i += unroll_factor) {
    #pragma HLS loop_tripcount max = READ_BUF_LEN/unroll_factor min = READ_BUF_LEN/unroll_factor
    #pragma HLS pipeline
    #pragma HLS dependence type=inter variable=data false
    sum += BinaryDotProduct<const data_t, unroll_factor, large_data_t>::sum(&in1[i], &in2[i]);
  }
  return sum;
}