cmake_minimum_required(VERSION 3.26)
project(model_viewer)

#make sure built libs are placed in top root folder to be accessible to exe file 
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set (DX_12 ON)

#add dx libraries only when using openg_gl 
add_subdirectory(libs/glew)
add_subdirectory(libs/glm)
add_subdirectory(libs/glfw)
add_subdirectory(libs/imgui)

set(USE_PRE_COMPILED_HEADERS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)
file(GLOB_RECURSE SRC_FILES src/*)

if (DX_12)
    add_executable(model_viewer main.cpp ${SRC_FILES})
else ()
    add_executable(model_viewer main.cpp ${SRC_FILES})
endif ()

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

if (MSVC)
    add_definitions(/permissive /D "_WINDOWS" /D "_UNICODE" /D "UNICODE")
endif ()

target_include_directories(model_viewer PRIVATE libs/include)
find_library(ASSIMP_LIB assimp libs/libs)
#add dx libraries only when using dx_12 
target_link_libraries(model_viewer PRIVATE glfw glm ${ASSIMP_LIB} libglew_static imgui Comdlg32.dll d3dcompiler.lib D3D12.lib dxgi.lib)


install(FILES ${CMAKE_SOURCE_DIR}/libs/libs/assimp-vc143-mt.dll DESTINATION  ${CMAKE_BINARY_DIR})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/res DESTINATION  ${CMAKE_BINARY_DIR})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/Shaders DESTINATION  ${CMAKE_BINARY_DIR})