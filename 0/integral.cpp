#include <iostream>
#include <cmath>
#include <utility>
#include <functional>
#include <vector>
#include <numeric>
#include <string>

#include <omp.h>

int MAX_THREADS = 1;

double compute_integral_impl(const std::function<double(double)> fun, const std::pair<double, double> bounds, const double precision) {
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

double compute_integral(const std::function<double(double)> fun, const std::pair<double, double> bounds, const double precision) {
  // std::vector<double> sums(MAX_THREADS, 0);
  double sum = 0;
  const double part_sz = (bounds.second - bounds.first) / MAX_THREADS;
#pragma omp parallel for num_threads(MAX_THREADS) reduction(+ : sum)
  for (int i = 0; i < MAX_THREADS; ++i) {
    double part_sum;
    if (i == MAX_THREADS - 1) {
      sum += compute_integral_impl(fun, {bounds.first + part_sz * i, bounds.second}, precision);
    } else {
      sum += compute_integral_impl(fun, {bounds.first + part_sz * i, bounds.first + part_sz * (i + 1)}, precision);
    }
#pragma omp critical
    std::cout << "sum " << i << ": " << sum << '\n';
    // #pragma omp critical
    // sum += part_sum;
  }
  // return std::accumulate(sums.cbegin(), sums.cend(), 0.0);
  return sum;
}

double f(double x) {
  return std::atan(std::pow(x, 3));
}

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: ./integral number_of_threads" << std::endl;
  }
  try {
    MAX_THREADS = std::stoi(argv[1]);
  } catch (std::exception& e) {
    std::cerr << "Error: " << e.what()<< std::endl;
    return 1;
  }
  if (MAX_THREADS < 1) {
    std::cerr << "Error: number_of_threads must be 1 or greater" << std::endl;
    return 1;
  }

  std::pair<double, double> bounds{-5000, 10000};
  double precision = 0.0001;
  double res = compute_integral(&f, bounds, precision);
  std::cout << "Integral for arctg(x^3) from " << bounds.first << " to " << bounds.second << " with precision " << precision << " is " << res << std::endl;
  return 0;
}
