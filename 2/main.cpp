#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

#include "diff_equation.hpp"

double f(double x, double y, double z) {
  return std::cos(3 * x) - 4 * y;
}
double g(double x, double y, double z) {
  return (z);
}
// Решение дифуров методом Рунге-Кутта
int main() {
  float x0 = 0;
  std::vector<float> y0i = {0.8, 2};
  float step = 0.001;
  int totalSteps = 10000;
  std::vector<std::function<float(float, float, float)>> fi = {
      [](float x, float y, float z) -> float {
        return std::cos(3 * x) - 4 * y;  // z' = cos(3x) - 4y
      },
      [](float x, float y, float z) -> float { return z; }  // y' = z
  };
  DiffEquation de(fi, x0, y0i, step);
  std::vector<Point> results(totalSteps, Point{});
  for (int i = 0; i < totalSteps; ++i) {
    results[i] = de.next();
  }
  for (auto& r : results) {
    std::cout << r.x << " ";
    for (auto y : r.y) {
      std::cout << y << " ";
    }
    std::cout << "\n";
  }
  return 0;
}
