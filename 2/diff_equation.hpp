#pragma once

#include <functional>
#include <vector>

struct Point {
  float x;
  std::vector<float> y;

  Point() = default;
  Point(float x, std::vector<float> y) : x(x), y(y){};
};

class DiffEquation {
  public:
  DiffEquation(std::vector<std::function<float(float, float, float)>> system,
               float x0,
               std::vector<float> y0i,
               float step);
  Point next();

  private:
  const int systemSize = 2;
  const std::vector<std::function<float(float, float, float)>> system;
  float xn;
  std::vector<float> yn;
  float h;
};
