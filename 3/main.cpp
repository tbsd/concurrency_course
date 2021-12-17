#include <cmath>
#include <functional>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <vector>
#include "mpi.h"

int MAX_THREADS = 3;

double compute_integral_impl(const std::function<double(double)> fun,
                             const std::pair<double, double> bounds,
                             const double precision) {
  double x1 = bounds.first;
  double sum = 0;
  while (x1 + precision < bounds.second) {
    double x2 = x1 + precision;
    sum += fun(x1) * (x2 - x1);
    x1 = x2;
  }
  sum += fun(x1) * (bounds.second - x1);
  return sum;
}

/*
double compute_integral(const std::function<double(double)> fun,
                        const std::pair<double, double> bounds,
                        const double precision) {
  // std::vector<double> sums(MAX_THREADS, 0);
  double sum = 0;
  const double part_sz = (bounds.second - bounds.first) / MAX_THREADS;
  for (int i = 0; i < MAX_THREADS; ++i) {
    double part_sum;
    if (i == MAX_THREADS - 1) {
      sum += compute_integral_impl(
          fun, {bounds.first + part_sz * i, bounds.second}, precision);
    } else {
      sum += compute_integral_impl(
          fun, {bounds.first + part_sz * i, bounds.first + part_sz * (i + 1)},
          precision);
    }
    std::cout << "sum " << i << ": " << sum << '\n';
  }
  return sum;
}
*/

double fun(double x) {
  return std::atan(std::pow(x, 3));
}
void compute_integral(const std::function<double(double)> fun,
                      const std::pair<double, double> bounds,
                      const double precision) {}

// mpiexec -np 3 ./3
int main(int argc, char** argv) {
  MAX_THREADS = 3;
  std::pair<double, double> bounds{-5000, 10000};
  double precision = 0.0001;
  MPI_Status status;
  MPI_Init(&argc, &argv);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  double part_result = 0;
  if (rank == 0) {
    part_result = compute_integral_impl(fun, {bounds.first, bounds.second / 3},
                                        precision);
  } else if (rank == 1) {
    part_result = compute_integral_impl(
        fun, {bounds.second / 3, bounds.second * 2 / 3}, precision);
  } else {
    part_result = compute_integral_impl(
        fun, {bounds.second * 2 / 3, bounds.second}, precision);
  }
  double total = part_result;
  int count = 3;
  int buff_sz = 2 + sizeof(double) + MPI_BSEND_OVERHEAD;
  auto buff = std::make_unique<unsigned char>(buff_sz);
  MPI_Request reqs[2];
  MPI_Status stats[2];
  MPI_Buffer_attach(buff.get(), count);
  for (int i = 0, k = 0; i < 3; ++i)
    if (i != rank)
      MPI_Ibsend(&part_result, 1, MPI_DOUBLE, i, rank, MPI_COMM_WORLD,
                 &reqs[k++]);
  MPI_Waitall(2, reqs, stats);
  double tmp;
  MPI_Recv(&tmp, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,
           &status);
  total += tmp;
  MPI_Recv(&tmp, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,
           &status);
  total += tmp;
  std::cout << "rank: " << rank << ", tatal intgral value: " << total
            << std::endl;
  MPI_Buffer_detach(buff.get(), &count);
  MPI_Finalize();
  return 0;
}
