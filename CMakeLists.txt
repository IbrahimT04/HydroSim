cmake_minimum_required(VERSION 3.22)
project(HydroSim LANGUAGES CXX)

add_executable(HydroSim
        src/main.cpp
        src/config.h
        src/VulkanEngine.h
        src/VulkanEngine.cpp
        src/logging.h
        src/instance_helpers.h
        src/device_helpers.h
        src/swapchain_helpers.h
        src/shader_loader.h
        src/buffer_helpers.h
)

set_property(TARGET HydroSim PROPERTY CXX_STANDARD 23)
set_property(TARGET HydroSim PROPERTY CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

add_subdirectory(external/glm)

target_link_libraries(HydroSim PRIVATE
        Vulkan::Vulkan
        glfw
        glm::glm
)

target_include_directories(HydroSim PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/external/other
)

find_program(SLANGC_EXECUTABLE NAMES slangc slangc.exe REQUIRED)
message(STATUS "Using slangc: ${SLANGC_EXECUTABLE}")

set(SHADER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/shaders")
set(SHADER_BINARY_DIR "${CMAKE_BINARY_DIR}/spir_v_shaders")
set(TEXTURE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/textures")
set(TEXTURE_BINARY_DIR "${CMAKE_BINARY_DIR}/textures")

file(MAKE_DIRECTORY "${SHADER_BINARY_DIR}")
file(MAKE_DIRECTORY "${TEXTURE_BINARY_DIR}")

file(GLOB SLANG_SHADER_FILES CONFIGURE_DEPENDS
        "${SHADER_SOURCE_DIR}/*.slang"
)

message(STATUS "Shader source dir: ${SHADER_SOURCE_DIR}")
message(STATUS "Shader files: ${SLANG_SHADER_FILES}")

if(NOT SLANG_SHADER_FILES)
    message(WARNING "No .slang shader files found in ${SHADER_SOURCE_DIR}")
endif()

set(SLANG_SHADER_TARGETS)

foreach(SHADER_FILE IN LISTS SLANG_SHADER_FILES)
    get_filename_component(SHADER_NAME "${SHADER_FILE}" NAME_WE)
    set(SHADER_OUTPUT "${SHADER_BINARY_DIR}/${SHADER_NAME}.spv")
    set(SHADER_TARGET "slang_shader_${SHADER_NAME}")

    add_custom_target(${SHADER_TARGET}
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${SHADER_BINARY_DIR}"
            COMMAND "${SLANGC_EXECUTABLE}"
            "${SHADER_FILE}"
            -target spirv
            -profile spirv_1_4
            -fvk-use-entrypoint-name
            -entry vertMain
            -entry fragMain
            -o "${SHADER_OUTPUT}"
            BYPRODUCTS "${SHADER_OUTPUT}"
            DEPENDS "${SHADER_FILE}"
            COMMENT "Compiling Slang shader ${SHADER_FILE} -> ${SHADER_OUTPUT}"
            VERBATIM
    )

    list(APPEND SLANG_SHADER_TARGETS ${SHADER_TARGET})
endforeach()

if(SLANG_SHADER_TARGETS)
    add_custom_target(HydroSimShaders ALL
            DEPENDS ${SLANG_SHADER_TARGETS}
    )
    add_dependencies(HydroSim HydroSimShaders)
endif()

add_custom_command(TARGET HydroSim POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${TEXTURE_BINARY_DIR}"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${TEXTURE_SOURCE_DIR}"
        "${TEXTURE_BINARY_DIR}"
        COMMENT "Copying textures to ${TEXTURE_BINARY_DIR}"
)