cmake_minimum_required(VERSION 3.22)
project(HydroSim LANGUAGES CXX)

add_executable(HydroSim src/main.cpp
        src/config.h
        src/VulkanEngine.h
        src/VulkanEngine.cpp
        src/logging.h
        src/instance_helpers.h
        src/device_helpers.h)

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

# ---- GLM (header-only) ----
add_subdirectory(external/glm)
target_link_libraries(HydroSim PRIVATE glm::glm)

# Link against targets (brings include dirs automatically)
target_link_libraries(HydroSim PRIVATE Vulkan::Vulkan glfw)

add_custom_target(Shaders ALL DEPENDS ${SHADER_OUTPUTS})
add_dependencies(HydroSim Shaders)