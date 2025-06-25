cmake_minimum_required(VERSION 3.10)

project(droneUI VERSION 1.0 DESCRIPTION "Drone UI based on ImGui" LANGUAGES CXX)

if(MSVC)
    add_compile_options("/utf-8")
endif()
file(GLOB_RECURSE SRC_FILES
        "${PROJECT_SOURCE_DIR}/src/*.h"
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.hpp"
        "appicon.rc"
)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -s --static")

add_executable(droneUI ${SRC_FILES})
if (WIN32)
    #set_target_properties(droneUI PROPERTIES WIN32_EXECUTABLE TRUE)
    set_target_properties(droneUI PROPERTIES
            RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/appicon.rc"
    )
endif()

add_subdirectory(lib)
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/lib/sdl3/include")
target_link_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/lib/sdl3/lib")

target_link_libraries(droneUI PUBLIC
        imgui
        glfw
        opengl32
        glu32
)

set_target_properties(droneUI PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 1
)
