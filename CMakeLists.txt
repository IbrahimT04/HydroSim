cmake_minimum_required(VERSION 3.20)
project(HydroSim LANGUAGES CXX)

add_executable(HydroSim src/main.cpp
        src/Engine.cpp
        src/Engine.h
        src/config.h
        src/instance.h
        src/logging.h
        src/logging.cpp
        src/device.h
        src/queues.h
        src/swapchain.h
        src/frame.h
        src/query_system.h
        src/shaders.h
        src/pipeline.h
        src/framebuffer.h
        src/commands.h
        src/sync.h
        src/app.h
        src/app.cpp)

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

# ---- Shader compilation (GLSL -> SPIR-V) ----
# Optional override for users:
set(HYDROSIM_GLSLC "" CACHE FILEPATH "Optional override path to glslc")

# Prefer Vulkan::glslc if FindVulkan provides it; fallback to override; then find_program.
set(GLSLC_CMD "")

if (TARGET Vulkan::glslc)
    set(GLSLC_CMD "$<TARGET_FILE:Vulkan::glslc>")
elseif (HYDROSIM_GLSLC)
    set(GLSLC_CMD "${HYDROSIM_GLSLC}")
else()
    find_program(GLSLC_CMD glslc
            HINTS
            "$ENV{VULKAN_SDK}/Bin"
            "$ENV{VULKAN_SDK}/Bin32"
            "$ENV{VULKAN_SDK}/bin"
    )
endif()

if (NOT GLSLC_CMD)
    message(FATAL_ERROR
            "glslc not found.\n"
            "Install the Vulkan SDK (recommended) or set -DHYDROSIM_GLSLC=\"C:/path/to/glslc.exe\"."
    )
else()
    message(STATUS "Using glslc: ${GLSLC_CMD}")
endif()


set(SHADER_SRC_DIR "${CMAKE_SOURCE_DIR}/src/shaders")
set(SHADER_BIN_DIR "${CMAKE_BINARY_DIR}/spir_v_shaders")

file(GLOB_RECURSE SHADER_SOURCES CONFIGURE_DEPENDS
        "${SHADER_SRC_DIR}/*.vert"
        "${SHADER_SRC_DIR}/*.frag"
        "${SHADER_SRC_DIR}/*.comp"
        "${SHADER_SRC_DIR}/*.geom"
        "${SHADER_SRC_DIR}/*.tesc"
        "${SHADER_SRC_DIR}/*.tese"
)

set(SHADER_OUTPUTS "")

# Optional: SPIR-V validator (from Vulkan SDK / SPIRV-Tools)
find_program(SPIRV_VAL spirv-val
        HINTS
        "$ENV{VULKAN_SDK}/Bin"
        "$ENV{VULKAN_SDK}/Bin32"
        "$ENV{VULKAN_SDK}/bin"
)

if (SPIRV_VAL)
    message(STATUS "Using spirv-val: ${SPIRV_VAL}")
else()
    message(STATUS "spirv-val not found (validation disabled)")
endif()


foreach(SHADER ${SHADER_SOURCES})
    file(RELATIVE_PATH REL_PATH "${SHADER_SRC_DIR}" "${SHADER}")
    set(OUT_SPV "${SHADER_BIN_DIR}/${REL_PATH}.spv")
    get_filename_component(OUT_DIR "${OUT_SPV}" DIRECTORY)

    add_custom_command(
            OUTPUT "${OUT_SPV}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${OUT_DIR}"
            COMMAND ${GLSLC_CMD} -O "${SHADER}" -o "${OUT_SPV}"
            DEPENDS "${SHADER}"
            COMMENT "Compiling shader ${REL_PATH} -> ${REL_PATH}.spv"
            VERBATIM
    )

    # Always build SPV in all configs
    if (SPIRV_VAL AND CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_custom_command(
                OUTPUT "${OUT_SPV}.validated"
                COMMAND "${SPIRV_VAL}" "${OUT_SPV}"
                COMMAND "${CMAKE_COMMAND}" -E touch "${OUT_SPV}.validated"
                DEPENDS "${OUT_SPV}"
                COMMENT "Validating SPIR-V ${REL_PATH}.spv"
                VERBATIM
        )
        list(APPEND SHADER_OUTPUTS "${OUT_SPV}.validated")
    else()
        list(APPEND SHADER_OUTPUTS "${OUT_SPV}")
    endif()
endforeach()


add_custom_target(Shaders ALL DEPENDS ${SHADER_OUTPUTS})
add_dependencies(HydroSim Shaders)

add_custom_command(TARGET HydroSim POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:HydroSim>/shaders"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${SHADER_BIN_DIR}"
        "$<TARGET_FILE_DIR:HydroSim>/shaders"
        COMMENT "Copying compiled shaders to runtime output folder"
)