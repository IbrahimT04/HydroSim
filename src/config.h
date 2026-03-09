//
// Created by itari on 2026-02-24.
//

#ifndef HYDROSIM_IMPORT_CONFIG_H
#define HYDROSIM_IMPORT_CONFIG_H

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#include <iostream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#endif //HYDROSIM_IMPORT_CONFIG_H