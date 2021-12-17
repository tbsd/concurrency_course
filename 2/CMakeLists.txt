cmake_minimum_required(VERSION 3.5)

project(1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(1_2
  main.cpp
  diff_equation.cpp
  )

set(CMAKE_CXX_FLAGS "-fopenmp")
