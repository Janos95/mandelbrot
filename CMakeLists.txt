cmake_minimum_required(VERSION 3.13)
project(mandelbrot)

find_package(OpenMP)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(CMAKE_CXX_STANDARD 17)

add_executable(mandelbrot mandelbrot.cpp)


target_link_libraries(mandelbrot CONAN_PKG::opencv OpenMP::OpenMP_CXX)