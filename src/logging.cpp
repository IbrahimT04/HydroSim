    //
    // Created by itari on 2026-01-26.
    //
#include "config.h"
#include <iostream>

namespace vkInit {

    VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
        vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        vk::DebugUtilsMessageTypeFlagsEXT        messageType,
        const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    ) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << '\n';
        return VK_FALSE; // apps should return false :contentReference[oaicite:2]{index=2}
    }

    vk::DebugUtilsMessengerEXT make_debug_messeger(const vk::Instance& instance, const vk::detail::DispatchLoaderDynamic& dldi) {
        constexpr auto createInfo = vk::DebugUtilsMessengerCreateInfoEXT{}
        .setMessageSeverity(
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
        .setMessageType(
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
        .setPfnUserCallback(&debugCallback)
        .setPUserData(nullptr);

        return instance.createDebugUtilsMessengerEXT(createInfo, nullptr, dldi);
    }

} // namespace vkInit
