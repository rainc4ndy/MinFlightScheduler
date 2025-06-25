cmake_minimum_required(VERSION 3.10)

project(droneUI VERSION 1.0 DESCRIPTION "Drone UI based on ImGui" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Release build settings
set(CMAKE_BUILD_TYPE Release)
if(MSVC)
    add_compile_options("/utf-8")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

# Source files
file(GLOB_RECURSE SRC_FILES
    "${PROJECT_SOURCE_DIR}/src/*.h"
    "${PROJECT_SOURCE_DIR}/src/*.hpp"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

# Add executable
add_executable(droneUI ${SRC_FILES})

# Windows-specific resource file
if(WIN32)
    target_sources(droneUI PRIVATE "${PROJECT_SOURCE_DIR}/appicon.rc")
    set_target_properties(droneUI PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# Add libraries
add_subdirectory(lib)

# Include and link directories
target_include_directories(droneUI PUBLIC "${PROJECT_SOURCE_DIR}/lib/sdl3/include")
target_link_directories(droneUI PUBLIC "${PROJECT_SOURCE_DIR}/lib/sdl3/lib")

# Platform-specific libraries
if(WIN32)
    target_link_libraries(droneUI PUBLIC
        imgui
        glfw
        opengl32
        glu32
    )
elseif(UNIX)
    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED)
    target_link_libraries(droneUI PUBLIC
        imgui
        glfw
        ${OPENGL_gl_LIBRARY}
        GLU
        X11
        pthread
        dl
        m
    )
endif()

# Set version info
set_target_properties(droneUI PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)
