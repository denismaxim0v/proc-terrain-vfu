cmake_minimum_required(VERSION 3.16)
project(ProceduralTerrainGeneration)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(vendor/glfw)

add_library(glad vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

file(GLOB IMGUI_SRC
    vendor/imgui/*.cpp
    vendor/imgui/backends/imgui_impl_glfw.cpp
    vendor/imgui/backends/imgui_impl_opengl3.cpp
)

add_library(imgui ${IMGUI_SRC})
target_include_directories(imgui PUBLIC
    vendor/imgui
    vendor/imgui/backends
    vendor/glfw/include
)

add_executable(ProceduralTerrainGeneration  src/main.cpp src/terrain.cpp src/shader.cpp src/camera.cpp)

target_include_directories(ProceduralTerrainGeneration PUBLIC
    vendor/glfw/include
    vendor/glad/include
    vendor/glm
)

target_link_libraries(ProceduralTerrainGeneration 
    glfw
    glad
    imgui
    opengl32
)

file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ProceduralTerrainGeneration)