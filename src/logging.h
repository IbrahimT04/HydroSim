//
// Created by itari on 2026-01-26.
//

#ifndef HYDROSIM_LOGGING_H
#define HYDROSIM_LOGGING_H

#include "config.h"

namespace vkInit {
    VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    vk::DebugUtilsMessengerEXT make_debug_messeger(const vk::Instance& instance, const vk::detail::DispatchLoaderDynamic& dldi);

    void log_device_properties(const vk::PhysicalDevice& device);

    std::vector<std::string> log_transform_bits(vk::SurfaceTransformFlagsKHR bits);

    std::vector<std::string> log_alpha_composite_bits(vk::CompositeAlphaFlagsKHR bits);

    std::vector<std::string> log_image_usage_bits(vk::ImageUsageFlags bits);
}

#endif //HYDROSIM_LOGGING_H