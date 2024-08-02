cmake_minimum_required(VERSION 3.0)

set (CMAKE_CXX_STANDARD 17)
project(project_name)

add_subdirectory(third_party)

set(SRC_DIR ./src/)
add_executable(executable_name
    "${SRC_DIR}/main.cpp"
)
find_package(OpenGL REQUIRED)
target_link_libraries(executable_name glfw imgui OpenGL::GL)
