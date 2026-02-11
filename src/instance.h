//
// Created by itari on 2026-01-25.
//

#ifndef HYDROSIM_INSTANCE_H
#define HYDROSIM_INSTANCE_H
#include "config.h"

namespace vkInit {

    inline bool supported(const std::vector<const char*>& extensions, const std::vector<const char*>& layers, const bool debug) {

        // Show all supported extensions
        const std::vector<vk::ExtensionProperties> supportedExtensions = vk::enumerateInstanceExtensionProperties();
        if (debug) {
            std::cout << "System can support the following extensions:\n";
            for (auto supportedExtension: supportedExtensions) {
                std::cout << '\t' << supportedExtension.extensionName << '\n';
            }
        }
        // Check if all extensions are supported
        for (const auto extension : extensions) {
            bool found = false;
            for (auto supportedExtension: supportedExtensions) {
                if (strcmp(extension, supportedExtension.extensionName) == 0) {
                    found = true;
                    if (debug) {
                        std::cout << "Extension \"" << extension << "\" is supported. \n";
                    }
                }
            }
            if (!found) {
                if (debug) {
                    std::cout << "Extension \"" << extension << "\" is not supported. \n";
                }
                return false;
            }
        }

        // Show all supported layers
        const std::vector<vk::LayerProperties> supportedLayers = vk::enumerateInstanceLayerProperties();
        if (debug) {
            std::cout << "System can support the following layers:\n";
            for (auto supportedLayer: supportedLayers) {
                std::cout << '\t' << supportedLayer.layerName << '\n';
            }
        }
        // Check if all layers are supported
        for (const auto layer : layers) {
            bool found = false;
            for (auto supportedLayer: supportedLayers) {
                if (strcmp(layer, supportedLayer.layerName) == 0) {
                    found = true;
                    if (debug) {
                        std::cout << "Layer \"" << layer << "\" is supported. \n";
                    }
                }
            }
            if (!found) {
                if (debug) {
                    std::cout << "Layer \"" << layer << "\" is not supported. \n";
                }
                return false;
            }
        }

        return true;

        return true;
    }

    inline vk::Instance make_instance(bool debug, const char* applicationName) {
        if (debug) {
            std::cout << "Making an instance for " << applicationName;
        }

        // Making a Vulkan handle (Like a pointer)
        uint32_t version{ 0 };
        vkEnumerateInstanceVersion(&version);

        // Print version info
        if (debug) {
            std::cout << "System can support vulkan Variant: " << VK_API_VERSION_VARIANT(version)
                << ", Major: " << VK_API_VERSION_MAJOR(version)
                << ", Minor: " << VK_API_VERSION_MINOR(version)
                << ", Patch: " << VK_API_VERSION_PATCH(version) << "\n";
        }

        // Set patch to 0 for maximum compatibility
        version &= ~(0xFFFU);

        // Or hard code the version.
        // version = VK_MAKE_API_VERSION(0, 1, 4, 335);

        // Declare Application info
        auto appInfo = vk::ApplicationInfo(
            applicationName,
            version,
            "MaxMachine",
            version,
            version);

        // Everything with Vulkan is opt-in so we need to add the vulkan extension for glfw ourselves

        // Add extensions
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        // Add extension debug utilities
        if (debug) {
            extensions.push_back("VK_EXT_debug_utils");
        }

        // Add validation layers
        std::vector<const char*> layers;
        if (debug) {
            layers.push_back("VK_LAYER_KHRONOS_validation");
        }

        // Print requested extensions
        if (debug) {
            std::cout << "Requesting Extensions: " << "\n";
            for (const char* ext : extensions) {
                std::cout << "\t\"" << ext << "\"\"\n";
            }
        }
        if (!supported(extensions, layers, debug)) {
            return nullptr;
        }

        // Make createInfo
        const auto createInfo = vk::InstanceCreateInfo(
            vk::InstanceCreateFlags(),
            &appInfo,
            static_cast<uint32_t>(layers.size()),layers.data(),
            static_cast<uint32_t>(extensions.size()),extensions.data()
        );

        // Create Instance
        try {
            return vk::createInstance(createInfo, nullptr);
        }
        catch (const vk::SystemError sys_error) {
            if (debug) {
                std::cout << "Failed to create instance: " << sys_error.what() << "\n";
            }
            return nullptr;
        }
    }
}

#endif //HYDROSIM_INSTANCE_H