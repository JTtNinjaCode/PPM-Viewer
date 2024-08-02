cmake_minimum_required(VERSION 3.0)

set (CMAKE_CXX_STANDARD 17)
project(ppm_viewer)

set(SRC_DIR ./src)
add_executable(ppm_viewer
    "${SRC_DIR}/main.cc"
)