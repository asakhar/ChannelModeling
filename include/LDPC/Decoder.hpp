#ifndef DECODER_HPP
#define DECODER_HPP

#include <bits/stdint-uintn.h>
#define CL_TARGET_OPENCL_VERSION 120
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iostream>
#include <random>
#include <vector>

#include "src/Timer.h"
#include "src/matrix_generate.h"
#include <boost/compute/algorithm/copy.hpp>
#include <boost/compute/container/vector.hpp>
#include <boost/compute/core.hpp>
#include <boost/compute/utility/source.hpp>
#include <cmath>

#include "unitproto.hpp"

namespace compute = boost::compute;

const int ARG_0 = 0;
const int ARG_1 = 1;
const int ARG_2 = 2;
const int ARG_3 = 3;
const int ARG_4 = 4;
const int ARG_5 = 5;
const int ARG_6 = 6;

struct DecoderMinSumByIndex
    : public UnitProto<std::vector<float>, std::vector<float>> {
  compute::device gpu;
  compute::context context;
  compute::command_queue queue;

  int a;
  int b;
  int l;
  int max_iterations;

  size_t n;
  size_t k;
  std::vector<int> check_matrix_of_index;
  std::vector<float> E;
  std::vector<int> syndrom;
  compute::vector<int> buffer_check_matrix;
  compute::vector<float> buffer_E;
  compute::vector<int> buffer_syndrom;
  std::vector<int> check_matrix;

  compute::kernel kernel_LLR;
  compute::kernel kernel_HS;
  compute::kernel kernel_VS;
  compute::kernel kernel_check;

public:
  DecoderMinSumByIndex(int ones_row, int ones_column, int length,
                       int max_iter = 100)
      : gpu{compute::system::default_device()}, context{gpu},
        queue{context, gpu}, a{ones_row}, b{ones_column}, l{length},
        max_iterations(max_iter), n(b * l), k(a * l),
        check_matrix_of_index(a * b * l), E(a * b * l), syndrom(k),
        buffer_E(a * b * l, context), buffer_syndrom(k, context),
        check_matrix(n * k),
        kernel_LLR(
            compute::program::build_with_source_file("KernelLLR.cl", context),
            "fromBitToLLR"),
        kernel_HS(compute::program::build_with_source_file(
                      "KernelHS_byindex.cl", context),
                  "horizontal step"),
        kernel_VS(compute::program::build_with_source_file(
                      "KernelVS_byindex.cl", context),
                  "vertical step"),
        kernel_check(compute::program::build_with_source_file(
                         "KernelCheck_byindex.cl", context),
                     "check")

  {
    check_matrix_generate_with_idxMatrix(a, b, l, check_matrix,
                                         check_matrix_of_index);
    int row_num = a * l;
    int col_num = b * l;
    gen_matrix(row_num, col_num, check_matrix);
    std::sort(check_matrix_of_index.begin(), check_matrix_of_index.end());
    buffer_check_matrix = compute::vector<int>(
        check_matrix_of_index.begin(), check_matrix_of_index.end(), queue);
  }

  void run() override {

    // matrix parameters
    // a = numbers of ones in a row
    // b = numbers of ones in a column
    int row_num = a * l;
    int col_num = b * l;

    compute::vector<float> buffer_codeword(input.begin(), input.end(), queue);


    kernel_LLR.set_arg(0, buffer_codeword.get_buffer());

    kernel_HS.set_arg(ARG_0, buffer_check_matrix.get_buffer());
    kernel_HS.set_arg(ARG_1, buffer_codeword.get_buffer());
    kernel_HS.set_arg(ARG_2, (int)n);
    kernel_HS.set_arg(ARG_3, (int)b);
    kernel_HS.set_arg(ARG_4, (int)l);
    kernel_HS.set_arg(ARG_5, buffer_E.get_buffer());

    kernel_VS.set_arg(ARG_0, buffer_codeword.get_buffer());
    kernel_VS.set_arg(ARG_1, (int)n);
    kernel_VS.set_arg(ARG_2, (int)a);
    kernel_VS.set_arg(ARG_3, (int)b);
    kernel_VS.set_arg(ARG_4, (int)l);
    kernel_VS.set_arg(ARG_5, buffer_E.get_buffer());
    kernel_VS.set_arg(ARG_6, buffer_check_matrix.get_buffer()); // нужно ли это делать каждый раз??

    kernel_check.set_arg(ARG_0, buffer_check_matrix.get_buffer()); // и это?
    kernel_check.set_arg(ARG_1, buffer_codeword.get_buffer());
    kernel_check.set_arg(ARG_2, (int)n);
    kernel_check.set_arg(ARG_3, (int)b);
    kernel_check.set_arg(ARG_4, buffer_syndrom.get_buffer());

    int iterations_number = 0;
    bool exit = true;
    // queue.enqueue_1d_range_kernel(kernel_check, 0, a, 0);
    for (; iterations_number < max_iterations; iterations_number++) {
      queue.enqueue_1d_range_kernel(kernel_check, 0, k, 0);
      compute::copy(buffer_syndrom.begin(), buffer_syndrom.end(),
                    syndrom.begin(), queue);
      for (auto &a : syndrom) {
        if (a != 0) {
          exit = false;
          break;
        }
        exit = true;
      }
      if (!exit) {
        queue.enqueue_1d_range_kernel(kernel_LLR, 0, n, 0);
        queue.enqueue_1d_range_kernel(kernel_HS, 0, a * b * l, 0);
        queue.enqueue_1d_range_kernel(kernel_VS, 0, n, 0);
      } else
        break;
    }
    output.resize(n);
    compute::copy(buffer_codeword.begin(), buffer_codeword.end(),
                  output.begin(), queue);
  }
};

#endif // DECODER_HPP
