cmake_minimum_required(VERSION 3.20)
project(HydroSim LANGUAGES CXX)

add_executable(HydroSim src/main.cpp
        src/Engine.cpp
        src/Engine.h
        src/config.h
        src/instance.h)

# Pick a standard your compiler supports
set_property(TARGET HydroSim PROPERTY CXX_STANDARD 23)
set_property(TARGET HydroSim PROPERTY CXX_STANDARD_REQUIRED ON)

# Vulkan (from Vulkan SDK)
find_package(Vulkan REQUIRED)

# Build GLFW from source inside your repo
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

# Link against targets (brings include dirs automatically)
target_link_libraries(HydroSim PRIVATE Vulkan::Vulkan glfw)
