//
// Created by itari on 2026-03-07.
//

#ifndef HYDROSIM_DEVICE_HELPERS_H
#define HYDROSIM_DEVICE_HELPERS_H

#include "config.h"
#include <map>

namespace vkDevice {
    inline bool is_physical_device_suitable(const vk::raii::PhysicalDevice &physicalDevice,
                                            const std::vector<const char *> &requiredDeviceExtension) {
        // Check if the physicalDevice supports the Vulkan 1.3 API version
        bool supportsVulkan1_3 = physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;

        // Check if any of the queue families support graphics operations
        auto queueFamilies = physicalDevice.getQueueFamilyProperties();
        bool supportsGraphics = std::ranges::any_of(queueFamilies, [](auto const &qfp) {
            return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
        });

        // Check if all required physicalDevice extensions are available
        auto availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
        bool supportsAllRequiredExtensions =
                std::ranges::all_of(requiredDeviceExtension,
                                    [&availableDeviceExtensions](auto const &requiredDevExt) {
                                        return std::ranges::any_of(availableDeviceExtensions,
                                                                   [requiredDevExt](
                                                               auto const &availableDeviceExtension) {
                                                                       return strcmp(
                                                                                  availableDeviceExtension.
                                                                                  extensionName,
                                                                                  requiredDevExt) == 0;
                                                                   });
                                    });

        // Check if the physicalDevice supports the required features (dynamic rendering and extended dynamic state)
        auto features =
                physicalDevice
                .getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features,
                    vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
        bool supportsRequiredFeatures = features.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
                                        features.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().
                                        extendedDynamicState;

        // Return true if the physicalDevice meets all the criteria
        return supportsVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures;
    }

    inline vk::raii::PhysicalDevice get_most_suitable_physical_device(
        const vk::raii::Instance &instance,
        const std::vector<const char *> &requiredDeviceExtension) {
        auto physicalDevices = instance.enumeratePhysicalDevices();

        const auto devIter = std::ranges::find_if(
            physicalDevices,
            [&](auto const &physicalDevice) {
                return is_physical_device_suitable(physicalDevice, requiredDeviceExtension);
            });

        if (devIter == physicalDevices.end()) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }

        return std::move(*devIter);
    }

    inline uint32_t get_graphics_index(const vk::raii::PhysicalDevice &physical_device) {
        std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physical_device.getQueueFamilyProperties();

        const auto graphicsQueueFamilyProperty = std::ranges::find_if(
            queueFamilyProperties, [](auto const &qfp) {
                return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0);
            });
        auto graphicsIndex = static_cast<uint32_t>(
            std::distance(queueFamilyProperties.begin(), graphicsQueueFamilyProperty)
        );
        return graphicsIndex;
    }

    inline vk::raii::Device make_logical_device(const vk::raii::PhysicalDevice &physical_device,
                                                const std::vector<const char *> &requiredDeviceExtension) {
        std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physical_device.getQueueFamilyProperties();

        auto graphicsIndex = get_graphics_index(physical_device);

        float queuePriority = 0.5f;
        vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
            .queueFamilyIndex = graphicsIndex,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority
        };

        // Create a chain of feature structures
        vk::StructureChain<vk::PhysicalDeviceFeatures2,
            vk::PhysicalDeviceVulkan13Features,
            vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain = {
            {}, // vk::PhysicalDeviceFeatures2 (empty for now)
            {.dynamicRendering = true}, // Enable dynamic rendering from Vulkan 1.3
            {.extendedDynamicState = true} // Enable extended dynamic state from the extension
        };

        vk::DeviceCreateInfo deviceCreateInfo{
            .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &deviceQueueCreateInfo,
            .enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtension.size()),
            .ppEnabledExtensionNames = requiredDeviceExtension.data()
        };

        return vk::raii::Device(physical_device, deviceCreateInfo);
    }
}
#endif //HYDROSIM_DEVICE_HELPERS_H
