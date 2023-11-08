cmake_minimum_required(VERSION 3.26)

#make sure built libs are placed in top root folder to be accessible to exe file 
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (USE_PRE_COMPILED_HEADERS ON)

project(model_viewer)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)


add_subdirectory(libs/glew)
add_subdirectory(libs/glm)
add_subdirectory(libs/glfw)

file(GLOB SRC_FILES src/*)
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


target_link_libraries(model_viewer PRIVATE glfw glm libglew_static)

file(COPY ${CMAKE_SOURCE_DIR}/res
        DESTINATION ${CMAKE_BINARY_DIR})
