cmake_minimum_required(VERSION 3.10)

project(pewpew)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(pewpew src/main.cc src/hittable_list.cc src/sphere.cc)