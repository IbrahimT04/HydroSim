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

# Link against targets (brings include dirs automatically)
target_link_libraries(HydroSim PRIVATE glm::glm)

# Link against targets (brings include dirs automatically)
target_link_libraries(HydroSim PRIVATE Vulkan::Vulkan glfw glm::glm)

target_include_directories(HydroSim PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/external/stb
)

find_program(SLANGC_EXECUTABLE NAMES slangc REQUIRED)
message(STATUS "Using slangc: ${SLANGC_EXECUTABLE}")

set(SHADER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/shaders")
set(SHADER_BINARY_DIR "${CMAKE_BINARY_DIR}/spir_v_shaders")
set(TEXTURE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/textures")
set(TEXTURE_BINARY_DIR "${CMAKE_BINARY_DIR}/textures")

file(MAKE_DIRECTORY "${SHADER_BINARY_DIR}")
file(MAKE_DIRECTORY "${TEXTURE_BINARY_DIR}")

function(add_slang_shader TARGET SHADER_FILE)
    get_filename_component(SHADER_NAME "${SHADER_FILE}" NAME_WE)
    set(SHADER_OUTPUT "${SHADER_BINARY_DIR}/${SHADER_NAME}.spv")

    add_custom_command(
            OUTPUT "${SHADER_OUTPUT}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${SHADER_BINARY_DIR}"
            COMMAND "${SLANGC_EXECUTABLE}"
            "${SHADER_FILE}"
            -target spirv
            -profile spirv_1_4
            -fvk-use-entrypoint-name
            -entry vertMain
            -entry fragMain
            -o "${SHADER_OUTPUT}"
            DEPENDS "${SHADER_FILE}"
            COMMENT "Compiling shader ${SHADER_FILE} -> ${SHADER_OUTPUT}"
            VERBATIM
    )

    target_sources(${TARGET} PRIVATE "${SHADER_OUTPUT}")
endfunction()

file(GLOB CONFIGURE_DEPENDS SLANG_SHADER_FILES
        "${SHADER_SOURCE_DIR}/*.slang"
)

set(ALL_SHADER_OUTPUTS)
foreach(SHADER_FILE IN LISTS SLANG_SHADER_FILES)
    get_filename_component(SHADER_NAME "${SHADER_FILE}" NAME_WE)
    set(SHADER_OUTPUT "${SHADER_BINARY_DIR}/${SHADER_NAME}.spv")

    add_custom_command(
            OUTPUT "${SHADER_OUTPUT}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${SHADER_BINARY_DIR}"
            COMMAND "${SLANGC_EXECUTABLE}"
            "${SHADER_FILE}"
            -target spirv
            -profile spirv_1_4
            -fvk-use-entrypoint-name
            -entry vertMain
            -entry fragMain
            -o "${SHADER_OUTPUT}"
            DEPENDS "${SHADER_FILE}"
            COMMENT "Compiling shader ${SHADER_FILE} -> ${SHADER_OUTPUT}"
            VERBATIM
    )

    list(APPEND ALL_SHADER_OUTPUTS "${SHADER_OUTPUT}")
endforeach()

add_custom_target(HydroSimShaders ALL DEPENDS ${ALL_SHADER_OUTPUTS})
add_dependencies(HydroSim HydroSimShaders)

add_custom_command(TARGET HydroSim POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${TEXTURE_BINARY_DIR}"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${TEXTURE_SOURCE_DIR}"
        "${TEXTURE_BINARY_DIR}"
        COMMENT "Copying textures to ${TEXTURE_BINARY_DIR}"
)