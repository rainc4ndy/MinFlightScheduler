message(STATUS "Adding libs")

add_subdirectory(glfw)

# ImGui
add_library(imgui STATIC
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_stdlib.cpp
)
target_include_directories(imgui PUBLIC
        imgui
        imgui/backends
)
target_include_directories(imgui PUBLIC "${PROJECT_SOURCE_DIR}/lib/glfw/include")
target_link_directories(imgui PUBLIC "${PROJECT_SOURCE_DIR}/lib/glfw/lib")

