cmake_minimum_required(VERSION 3.10)

project(pewpew)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# For `std::jthread` and `std::stop_token`
# (https://libcxx.llvm.org/Status/Cxx20.html#note-p0660).
add_compile_options(-fexperimental-library)

add_executable(pewpew
               src/main.cc
               src/app.cc
               src/camera.cc
               src/dielectric.cc
               src/hittable_list.cc
               src/lambertian.cc
               src/metal.cc
               src/sphere.cc)

# SDL2
find_package(SDL2 REQUIRED)
target_link_libraries(pewpew ${SDL2_LIBRARIES})

# ImGui
add_library(imgui
            third_party/imgui/imgui.cpp
            third_party/imgui/imgui_demo.cpp
            third_party/imgui/imgui_draw.cpp
            third_party/imgui/imgui_tables.cpp
            third_party/imgui/imgui_widgets.cpp
            third_party/imgui/backends/imgui_impl_sdl2.cpp
            third_party/imgui/backends/imgui_impl_sdlrenderer2.cpp)
target_include_directories(imgui PUBLIC
                           third_party/imgui
                           third_party/imgui/backends)
target_link_libraries(imgui ${SDL2_LIBRARIES})
target_link_libraries(pewpew imgui)

# OpenMP
find_package(OpenMP REQUIRED)
target_link_libraries(pewpew OpenMP::OpenMP_CXX)

# PCG
target_include_directories(pewpew PRIVATE third_party/pcg-cpp/include)