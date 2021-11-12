#include "diff_equation.hpp"

#include <omp.h>

DiffEquation::DiffEquation(
    std::vector<std::function<float(float, float, float)>> system,
    float x0,
    std::vector<float> y0i,
    float step)
    : system(system), xn(x0), yn(y0i), h(step){};

Point DiffEquation::next() {
  std::vector<float> k1 = {0, 0}, k2 = {0, 0}, k3 = {0, 0}, k4 = {0, 0};
  auto ni = [this](int i) { return (i + 1) % systemSize; };  // next index
#pragma omp parallel for
  for (int i = 0; i < systemSize; ++i)
    k1[i] = h * system[i](xn, yn[0], yn[1]);
#pragma omp parallel for
  for (int i = 0; i < systemSize; ++i)
    k3[i] = h * system[i](xn + h / 2, yn[0] + k2[1] / 2, yn[1] + k2[0] / 2);
#pragma omp parallel for
  for (int i = 0; i < systemSize; ++i)
    k4[i] = h * system[i](xn + h, yn[0] + k3[1], yn[1] + k3[0]);
#pragma omp parallel for
  for (int i = 0; i < systemSize; ++i)
    yn[i] =
        yn[i] + (k1[ni(i)] + 2 * k2[ni(i)] + 2 * k3[ni(i)] + k4[ni(i)]) / 6.0;
  xn += h;
  return {xn, yn};
}
