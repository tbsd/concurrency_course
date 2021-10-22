#include <iomanip>
#include <iostream>

#include "diff_equation.hpp"

// Решение дифуров методом Рунге-Кутта
int main() {
  float x0 = 0;
  float y0 = 1;
  float step = 0.1;
  int totalSteps = 100;
  // y(x) = -x^2 - 2x + 3e^x - 2
  auto dy = [](float x, float y) -> float { return x * x + y; };
  std::cout << "y' = f(x, y) = x^2 + y, y(" << x0 << ") = " << y0
            << ", step = " << step << ", total steps = " << totalSteps
            << std::endl;
  DiffEquation de{dy, x0, y0, step};
  for (int i = 1; i <= totalSteps; ++i) {
    auto solution = de.next();
    std::cout << std::setprecision(10) << std::fixed << "x_" << i << " = "
              << solution.first << "    y_" << i << " = " << solution.second
              << std::endl;
  }
  return 0;
}
