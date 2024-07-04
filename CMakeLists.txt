cmake_minimum_required(VERSION 3.10)

project(pewpew)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(pewpew
               src/main.cc
               src/camera.cc
               src/dielectric.cc
               src/hittable_list.cc
               src/lambertian.cc
               src/metal.cc
               src/sphere.cc)

# PCG
target_include_directories(pewpew PRIVATE third_party/pcg-cpp/include)