//
// Created by itari on 2026-03-01.
//

#ifndef HYDROSIM_INSTANCE_HELPERS_H
#define HYDROSIM_INSTANCE_HELPERS_H

#include "config.h"

namespace vkInit {
    inline void print_available_extensions(const vk::raii::Context &context) {
        const auto extensions = context.enumerateInstanceExtensionProperties();

        std::cout << "Available extensions:\n";

        for (const auto &extension: extensions) {
            std::cout << '\t' << extension.extensionName << '\n';
        }
    }

    inline void print_available_validation_layers(const vk::raii::Context &context) {
        const auto validation_layers = context.enumerateInstanceLayerProperties();

        std::cout << "Available validation layers:\n";

        for (const auto &layer: validation_layers) {
            std::cout << '\t' << layer.layerName << '\n';
        }
    }

    inline std::vector<const char *> getRequiredInstanceExtensions(const bool enableValidationLayers) {
        uint32_t glfwExtensionCount = 0;

        const auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        if (enableValidationLayers) extensions.push_back(vk::EXTDebugUtilsExtensionName);

        return extensions;
    }

    inline bool extensions_supported(const vk::raii::Context &context, std::vector<char const *> requiredExtensions) {
        // Check if the required extensions are supported by the Vulkan implementation.
        auto extensionProperties = context.enumerateInstanceExtensionProperties();
        const auto unsupportedPropertyIt =
                std::ranges::find_if(requiredExtensions,
                                     [&extensionProperties](auto const &requiredExtension) {
                                         return std::ranges::none_of(extensionProperties,
                                                                     [requiredExtension
                                                                     ](auto const &extensionProperty) {
                                                                         return strcmp(extensionProperty.extensionName,
                                                                             requiredExtension) == 0;
                                                                     });
                                     });
        if (unsupportedPropertyIt != requiredExtensions.end()) {
            throw std::runtime_error("Required extension not supported: " + std::string(*unsupportedPropertyIt));
        }
        return true;
    }

    inline bool validation_layers_supported(const vk::raii::Context &context,
                                            std::vector<char const *> requiredLayers) {
        // Check if the required layers are supported by the Vulkan implementation.
        auto layerProperties = context.enumerateInstanceLayerProperties();

        const auto unsupportedLayerIt = std::ranges::find_if(requiredLayers,
                                                             [&layerProperties](auto const &requiredLayer) {
                                                                 return std::ranges::none_of(layerProperties,
                                                                     [requiredLayer](auto const &layerProperty) {
                                                                         return strcmp(layerProperty.layerName,
                                                                             requiredLayer) == 0;
                                                                     });
                                                             });
        if (unsupportedLayerIt != requiredLayers.end()) {
            throw std::runtime_error("Required layer not supported: " + std::string(*unsupportedLayerIt));
        }
        return true;
    }
}

#endif //HYDROSIM_INSTANCE_HELPERS_H
