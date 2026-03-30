cmake_minimum_required(VERSION 3.22)
project(HydroSim LANGUAGES CXX)

add_executable(HydroSim src/main.cpp
        src/config.h
        src/VulkanEngine.h
        src/VulkanEngine.cpp
        src/logging.h
        src/instance_helpers.h
        src/device_helpers.h
        src/swapchain_helpers.h
        src/shader_loader.h
        src/buffer_helpers.h)

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

find_program(SLANGC_EXECUTABLE NAMES slangc REQUIRED)
message(STATUS "Using slangc: ${SLANGC_EXECUTABLE}")

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/spir_v_shaders")

function(add_slang_shader_target TARGET)
    cmake_parse_arguments(SHADER "" "SOURCE;OUTPUT" "ENTRY_POINTS;INCLUDE_DIRS" ${ARGN})

    if (NOT SHADER_SOURCE)
        message(FATAL_ERROR "add_slang_shader_target: SOURCE is required")
    endif()

    if (NOT SHADER_OUTPUT)
        set(SHADER_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/shaders/${TARGET}.spv")
    endif()

    get_filename_component(OUT_DIR "${SHADER_OUTPUT}" DIRECTORY)

    set(SLANG_ARGS
            "${SHADER_SOURCE}"
            -target spirv
            -profile spirv_1_4
            -fvk-use-entrypoint-name
            -o "${SHADER_OUTPUT}"
    )

    foreach(ENTRY IN LISTS SHADER_ENTRY_POINTS)
        list(APPEND SLANG_ARGS -entry "${ENTRY}")
    endforeach()

    foreach(INC IN LISTS SHADER_INCLUDE_DIRS)
        list(APPEND SLANG_ARGS -I "${INC}")
    endforeach()

    add_custom_command(
            OUTPUT "${SHADER_OUTPUT}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${OUT_DIR}"
            COMMAND "${SLANGC_EXECUTABLE}" ${SLANG_ARGS}
            DEPENDS "${SHADER_SOURCE}"
            COMMENT "Compiling Slang shader module ${SHADER_SOURCE} -> ${SHADER_OUTPUT}"
            VERBATIM
    )

    add_custom_target(${TARGET} ALL DEPENDS "${SHADER_OUTPUT}")
endfunction()

add_slang_shader_target(HydroSimShaders
        SOURCE "${CMAKE_SOURCE_DIR}/src/shaders/default.slang"
        OUTPUT "${CMAKE_BINARY_DIR}/spir_v_shaders/default.spv"
        ENTRY_POINTS vertMain fragMain
)

add_dependencies(HydroSim HydroSimShaders)