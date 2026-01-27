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
}

#endif //HYDROSIM_LOGGING_H