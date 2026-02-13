//
// Created by itari on 2026-01-26.
//

#ifndef HYDROSIM_DEVICE_H
#define HYDROSIM_DEVICE_H

#include "config.h"
#include "queues.h"
#include "logging.h"
#include "set"

namespace vkInit {

    inline bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const std::vector<const char*>& requestedExtensions, const bool debug) {
        std::set<std::string> requiredExtensions(requestedExtensions.begin(), requestedExtensions.end());

        if (debug) {
            std::cout << "Device can support these extensions:\n";
        }
        for (auto& extension: device.enumerateDeviceExtensionProperties()) {
            if (debug) {
                std::cout << "\t" << extension.extensionName << "\n";
            }
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    inline bool isSuitable(const vk::PhysicalDevice& device, const bool debug) {
        if (debug) {
            std::cout << "Checking if device is suitable\n";
        }
        const std::vector requestedExtensions {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };

        if (debug) {
            std::cout << "Requesting Device Extensions: \n";
            for (const char* extension : requestedExtensions) {
                std::cout << "\t\"" << extension << "\"\n";
            }
        }
        if (checkDeviceExtensionSupport(device, requestedExtensions, debug)) {
            if (debug) {
                std::cout << "Device supports the requested extensions\n";
            }
        }
        else {
            if (debug) {
                std::cout << "Device does not support the requested extensions\n";
            }
            return false;
        }
        return true;
    }

    inline vk::PhysicalDevice choose_physical_device(const vk::Instance& instance, const bool debug) {

        if (debug) {
            std::cout << "Choosing physical devices -->\n";
        }

        const std::vector<vk::PhysicalDevice> availableDevices = instance.enumeratePhysicalDevices();

        if (debug) {
            std::cout << "Found " << availableDevices.size() << " physical devices in this system\n";
        }
        for (const vk::PhysicalDevice device : availableDevices) {

            if (debug) {
                log_device_properties(device);
            }
            if (isSuitable(device, debug) && device.getProperties().deviceType==vk::PhysicalDeviceType::eDiscreteGpu) {
                // Option to choose dedicated GPU.
                return device;
            }  /*
            if (isSuitable(device, debug)) { // Option to choose any GPU instead of the dedicated GPU.
                return device;
            } */
        }

        return nullptr;
    }

    inline vk::Device create_logical_device(const vk::PhysicalDevice& physical_device, const vk::SurfaceKHR& surface, const bool debug) {

        QueueFamilyIndices indices = findQueueFamilies(physical_device, surface, debug);
        std::vector<uint32_t> uniqueIndices;
        uniqueIndices.push_back(indices.graphicsFamily.value());
        if (indices.graphicsFamily.value() != indices.presentFamily.value()) {
            uniqueIndices.push_back(indices.presentFamily.value());
        }

        constexpr float queuePriority = 1.0f;

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo;
        for (uint32_t queueFamilyIndex : uniqueIndices) {
            queueCreateInfo.emplace_back(
                    vk::DeviceQueueCreateFlags(),
                    queueFamilyIndex,
                    1,
                    &queuePriority
                );
        }
        auto deviceFeatures = vk::PhysicalDeviceFeatures();
        // deviceFeatures.samplerAnisotropy = true; // Set the physical device features.

        std::vector<const char*> enabledLayers;
        if (debug) {
            enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
        }

        std::vector deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        auto deviceInfo = vk::DeviceCreateInfo(
            vk::DeviceCreateFlags(),
            queueCreateInfo.size(), queueCreateInfo.data(),
            enabledLayers.size(), enabledLayers.data(),
            deviceExtensions.size(), deviceExtensions.data(),
            &deviceFeatures
            );

        try {
            vk::Device device = physical_device.createDevice(deviceInfo);
            if (debug) {
                std::cout << "Successfully created logical device from GPU\n";
            }
            return device;
        }
        catch (const vk::SystemError& e) {
            if (debug) {
                std::cout << "Error creating logical device: " << e.what() << std::endl;
            }
            return nullptr;
        }
    }
}

#endif //HYDROSIM_DEVICE_H