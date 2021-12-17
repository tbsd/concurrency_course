#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <vector>

#include <cuda.h>
#include <cuda_runtime_api.h>

double f(double x, double y, double z) {
  return std::cos(3 * x) - 4 * y;
}
double g(double x, double y, double z) {
  return (z);
}

__global__ void calculateK1(float* k1,
                            const float* a,
                            const float* b,
                            float xn,
                            float yn,
                            float h) {
  int i = blockDim.x * blockIdx.x + threadIdx.x;
  k1[i] = h * (std::cos(a[i] * xn) - b[i] * yn);  // z' = cos(ax) - by
}

__global__ void calculateK2(float* k2,
                            const float* a,
                            const float* b,
                            const float* k1,
                            float xn,
                            float yn,
                            float h) {
  int i = blockDim.x * blockIdx.x + threadIdx.x;
  k2[i] = h * (std::cos(a[i] * (xn + h / 2)) - b[i] * (yn + k1[i] * h / 2));
}

__global__ void calculateK3(float* k3,
                            const float* a,
                            const float* b,
                            const float* k2,
                            float xn,
                            float yn,
                            float h) {
  int i = blockDim.x * blockIdx.x + threadIdx.x;
  k3[i] = h * (std::cos(a[i] * (xn + h / 2)) - b[i] * (yn + k2[i] * h / 2));
}

__global__ void calculateK4(float* k4,
                            const float* a,
                            const float* b,
                            const float* k3,
                            float xn,
                            float yn,
                            float h) {
  int i = blockDim.x * blockIdx.x + threadIdx.x;
  k4[i] = h * (std::cos(a[i] * (xn + h)) - b[i] * (yn + k3[i] * h));
}
__global__ void calculateYn(float* yn,
                            const float* a,
                            const float* b,
                            const float* k1,
                            const float* k2,
                            const float* k3,
                            const float* k4,
                            float h) {
  int i = blockDim.x * blockIdx.x + threadIdx.x;
  yn[i] = yn[i] + (k1[i] + 2 * k2[i] + 2 * k3[i] + k4[i]) / 6;
}

// Решение дифуров методом Рунге-Кутта
#define BLOCK_SIZE 256
// nvcc --ptxas-options=-v  main.cu -o prog
int main() {
  float x0 = 0;
  float xn = x0;
  std::vector<float> yn = {0.8, 2};
  float h = 0.001;
  int totalSteps = 10000;
  const int system_size = 100;
  std::time_t seed = std::time(nullptr);
  srand(seed);
  std::cout << "Seed: " << seed << std::endl;

  float* d_k1;
  float* d_k2;
  float* d_k3;
  float* d_k4;
  float* d_yn;
  float* d_a;
  float* d_b;
  int N = 100;  // BLOCK_SIZE * 100;
  size_t size = N * sizeof(float);
  float* h_k1 = (float*)malloc(size);
  float* h_k2 = (float*)malloc(size);
  float* h_k3 = (float*)malloc(size);
  float* h_k4 = (float*)malloc(size);
  float* h_yn = (float*)malloc(size);
  float* h_a = (float*)malloc(size);
  float* h_b = (float*)malloc(size);
  for (int i = 0; i < N; ++i) {
    h_a[i] = rand() % 1000;
    h_b[i] = rand() % 1000;
  }

  int threadsPerBlock = BLOCK_SIZE;
  int blocksPerGrid = N / threadsPerBlock;
  cudaMalloc((void**)&d_a, size);
  cudaMalloc((void**)&d_b, size);
  cudaMemcpy(d_a, h_a, size, cudaMemcpyHostToDevice);
  cudaMemcpy(d_b, h_b, size, cudaMemcpyHostToDevice);
  for (int i = 0; i < totalSteps; ++i) {
    cudaMalloc((void**)&d_k1, size);
    calculateK1<<<blocksPerGrid, threadsPerBlock>>>(d_k1, d_a, d_b, xn, yn[0],
                                                    h);
    cudaMemcpy(h_k1, d_k1, size, cudaMemcpyDeviceToHost);

    cudaMemcpy(d_k1, h_k1, size, cudaMemcpyHostToDevice);
    cudaMalloc((void**)&d_k2, size);
    calculateK2<<<blocksPerGrid, threadsPerBlock>>>(d_k2, d_a, d_b, d_k1, xn,
                                                    yn[0], h);
    cudaMemcpy(h_k2, d_k2, size, cudaMemcpyDeviceToHost);

    cudaMemcpy(d_k2, h_k2, size, cudaMemcpyHostToDevice);
    cudaMalloc((void**)&d_k3, size);
    calculateK3<<<blocksPerGrid, threadsPerBlock>>>(d_k3, d_a, d_b, d_k2, xn,
                                                    yn[0], h);
    cudaMemcpy(d_k3, h_k3, size, cudaMemcpyDeviceToHost);

    cudaMemcpy(d_k3, h_k3, size, cudaMemcpyHostToDevice);
    cudaMalloc((void**)&d_k4, size);
    calculateK4<<<blocksPerGrid, threadsPerBlock>>>(d_k4, d_a, d_b, d_k3, xn,
                                                    yn[0], h);
    cudaMemcpy(d_k4, h_k4, size, cudaMemcpyDeviceToHost);

    cudaMemcpy(d_k4, h_k4, size, cudaMemcpyHostToDevice);
    cudaMalloc((void**)&d_yn, size);
    calculateYn<<<blocksPerGrid, threadsPerBlock>>>(d_yn, d_a, d_b, d_k1, d_k2,
                                                    d_k3, d_k4, h);
    cudaMemcpy(h_yn, d_yn, size, cudaMemcpyDeviceToHost);

    xn += h;

    for (int j = 0; j < N; j++)
      std::cout << h_yn[j] << " ";
    std::cout << std::endl;
  }
  //  DiffEquation de(system, x0, y0i, h);
  //  std::vector<Point> results(totalSteps, Point{});
  //  for (int i = 0; i < totalSteps; ++i) {
  //    results[i] = de.next();
  //  }
  //  for (auto& r : results) {
  //    std::cout << r.x << " ";
  //    for (auto y : r.y) {
  //      std::cout << y << " ";
  //    }
  //    std::cout << "\n";
  //  }
  free(h_k1);
  free(h_k2);
  free(h_k3);
  free(h_k4);
  free(h_yn);
  free(h_a);
  free(h_b);
  return 0;
}
