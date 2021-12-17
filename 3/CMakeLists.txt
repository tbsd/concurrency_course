cmake_minimum_required(VERSION 3.5)

project(3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(MPI REQUIRED)

add_executable(3
  main.cpp
  )

target_link_libraries(3 MPI::MPI_CXX)

#set(CMAKE_CXX_FLAGS "-lbproc -Impi")
