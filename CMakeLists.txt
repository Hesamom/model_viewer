cmake_minimum_required(VERSION 3.26)
project(model_viewer)

#make sure built libs are placed in top root folder to be accessible to exe file 
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


set(USE_PRE_COMPILED_HEADERS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)


add_subdirectory(libs/glm)
add_subdirectory(libs/imgui)

file(GLOB_RECURSE SRC_FILES src/*)
file(GLOB_RECURSE DX_FILES src/render/dx/*)
file(GLOB_RECURSE GL_FILES src/render/gl/*)
file(GLOB_RECURSE DX_FILES_WINDOW src/window/dx/*)
file(GLOB_RECURSE GL_FILES_WINDOW src/window/gl/*)


if (dx_12)
    add_compile_definitions("GFX_DX")
    find_library(WinPix WinPixEventRuntime libs/libs)
    list(REMOVE_ITEM SRC_FILES ${GL_FILES} ${GL_FILES_WINDOW})
    add_executable(model_viewer main.cpp ${SRC_FILES})
    target_link_libraries(model_viewer PRIVATE d3dcompiler.lib D3D12.lib dxgi.lib ${WinPix})
else ()
    add_compile_definitions("GFX_GL")
    list(REMOVE_ITEM SRC_FILES ${DX_FILES}  ${DX_FILES_WINDOW})
    add_subdirectory(libs/glew)
    add_subdirectory(libs/glfw)
    add_executable(model_viewer main.cpp ${SRC_FILES})
    target_link_libraries(model_viewer PRIVATE glfw libglew_static)
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
target_link_libraries(model_viewer PRIVATE glm ${ASSIMP_LIB} Comdlg32 imgui)


if (dx_12)
    install(FILES ${CMAKE_SOURCE_DIR}/libs/libs/WinPixEventRuntime.dll DESTINATION  ${CMAKE_BINARY_DIR})
endif ()

install(FILES ${CMAKE_SOURCE_DIR}/libs/libs/assimp-vc143-mt.dll DESTINATION  ${CMAKE_BINARY_DIR})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/res DESTINATION  ${CMAKE_BINARY_DIR})