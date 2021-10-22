#pragma once

#include <functional>

class DiffEquation {
  public:
  DiffEquation(std::function<float(float, float)> f,
               float x0,
               float y0,
               float step);
  std::pair<float, float> next();  // y_(n+1)

  private:
  std::function<float(float, float)> f;  /// y' = f(x, y)
  float x_n;
  float y_n;
  float h;  /// step
};
