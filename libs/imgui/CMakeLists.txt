project(imgui)
file(GLOB_RECURSE SRC_FILES *)
add_library(imgui STATIC ${SRC_FILES})
target_link_libraries(imgui PRIVATE glfw)