cmake_minimum_required(VERSION 3.26)

#make sure built libs are placed in top root folder to be accessible to exe file 
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

project(model_viewer)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

add_subdirectory(libs/glew)
add_subdirectory(libs/glm)
add_subdirectory(libs/glfw)
add_executable(model_viewer main.cpp)

target_link_libraries(model_viewer PRIVATE glfw glm libglew_static)