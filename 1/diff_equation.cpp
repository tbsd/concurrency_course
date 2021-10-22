#include "diff_equation.hpp"

DiffEquation::DiffEquation(std::function<float(float, float)> f,
                           float x0,
                           float y0,
                           float step)
    : f(f), x_n(x0), y_n(y0), h(step) {}

std::pair<float, float> DiffEquation::next() {
  float k1 = f(x_n, y_n);
  float k2 = f(x_n + h / 2, y_n + h / 2 * k1);
  float k3 = f(x_n + h / 2, y_n + h / 2 * k2);
  float k4 = f(x_n + h, y_n + h * k3);
  y_n = y_n + h * (k1 + 2 * k2 + 2 * k3 + k4) / 6;
  x_n += h;
  return {x_n, y_n};
}
