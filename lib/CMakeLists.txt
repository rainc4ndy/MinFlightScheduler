message(STATUS "Adding libs")

add_subdirectory(glfw)

# ImGui
add_library(imgui STATIC
        libimgui/backends/imgui_impl_glfw.cpp
        libimgui/backends/imgui_impl_opengl3.cpp
        libimgui/imgui.cpp
        libimgui/imgui_draw.cpp
        libimgui/imgui_demo.cpp
        libimgui/imgui_tables.cpp
        libimgui/imgui_widgets.cpp
        libimgui/imgui_stdlib.cpp
)
target_include_directories(imgui PUBLIC
        libimgui
        libimgui/backends
)
target_include_directories(imgui PUBLIC "${PROJECT_SOURCE_DIR}/lib/glfw/include")
target_link_directories(imgui PUBLIC "${PROJECT_SOURCE_DIR}/lib/glfw/lib")

