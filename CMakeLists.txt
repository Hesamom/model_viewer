cmake_minimum_required(VERSION 3.26)
project(model_viewer)

set(CMAKE_CXX_STANDARD 20)


add_subdirectory(libs/glew)
add_subdirectory(libs/glm)
add_subdirectory(libs/glfw)


link_libraries(libglew_static)
link_libraries(glm)
link_libraries(glfw)

add_executable(model_viewer main.cpp)
