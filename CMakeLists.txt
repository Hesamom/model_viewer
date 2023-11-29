cmake_minimum_required(VERSION 3.26)
project(model_viewer)

#make sure built libs are placed in top root folder to be accessible to exe file 
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})



add_subdirectory(libs/glew)
add_subdirectory(libs/glm)
add_subdirectory(libs/glfw)
add_subdirectory(libs/assimp)

set (USE_PRE_COMPILED_HEADERS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)
file(GLOB_RECURSE SRC_FILES src/*)
add_executable(model_viewer main.cpp ${SRC_FILES})


if (USE_PRE_COMPILED_HEADERS)
    target_precompile_headers(model_viewer PRIVATE std_pch.h)
else ()
    if(MSVC)
        add_definitions(/FI"std_pch.h")
    else()
        # GCC or Clang
        add_definitions(-include std_pch.h)
    endif ()
endif ()

target_include_directories(model_viewer PRIVATE libs/include)

target_link_libraries(model_viewer PRIVATE glfw glm assimp libglew_static)


#todo find a way to copy this after every build not every time cmake is reloaded!
file(COPY ${CMAKE_SOURCE_DIR}/res
        DESTINATION ${CMAKE_BINARY_DIR})
