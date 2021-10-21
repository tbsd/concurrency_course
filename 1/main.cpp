#include <cmath>
#include <functional>
#include <iostream>

using namespace std;

class DiffEquation {
  public:
  DiffEquation(function<float(float, float)> f, float x0, float y0, float step)
      : f(f), x_n(x0), y_n(y0), h(step){};
  float next() {
    float k1 = f(x_n, y_n);
    float k2 = f(x_n + h / 2, y_n + h / 2 * k1);
    float k3 = f(x_n + h / 2, y_n + h / 2 * k2);
    float k4 = f(x_n + h, y_n + h * k3);
    y_n = y_n + h * (k1 + 2 * k2 + 2 * k3 + k4) / 6;
    x_n += h;
    return y_n;
  }  // y_(n+1)

  private:
  function<float(float, float)> f;  /// y' = f(x, y)
  float x_n;
  float y_n;
  float h;  /// step
};

// Решение дифуров методом Рунге-Кутта
int main() {
  float x0 = 1;
  float y0 = 4;
  float step = 0.01;
  int totalSteps = 100;
  auto f = [](float x, float y) -> float { return x * cosf(y); };
  cout << "y' = f(x, y) = x * cos(y), x0 = " << x0 << ", y0 = " << y0
       << ", step = " << step << ", total steps = " << totalSteps << endl;
  DiffEquation de{f, x0, y0, step};
  for (int i = 0; i < totalSteps; ++i)
    cout << "y_" << i + 1 << " = " << de.next() << endl;
  return 0;
}
