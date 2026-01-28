//
// Created by itari on 2026-01-26.
//

#ifndef HYDROSIM_DEVICE_H
#define HYDROSIM_DEVICE_H

#include "config.h"

namespace vkInit {
    inline void log_device_properties(const vk::PhysicalDevice& device) {
        vk::PhysicalDeviceProperties properties = device.getProperties();

        std::cout << "Device Name: " << properties.deviceName << "\n";
        std::cout << "Device Type: ";
        switch (properties.deviceType) {
            case (vk::PhysicalDeviceType::eCpu):
                std::cout << "CPU\n";
                break;
            case (vk::PhysicalDeviceType::eDiscreteGpu):
                std::cout << "Discrete GPU\n";
                break;
            case (vk::PhysicalDeviceType::eIntegratedGpu):
                std::cout << "Integrated GPU\n";
                break;
            case (vk::PhysicalDeviceType::eVirtualGpu):
                std::cout << "Virtual GPU\n";
                break;
            case (vk::PhysicalDeviceType::eOther):
                std::cout << "Other GPU\n";
                break;
            default:
                std::cout << "UNKNOWN\n";
        }
    }

    inline bool checkDeviceExtensionSupport(vk::PhysicalDevice& device, const std::vector<const char*> & requestedExtensions, const bool debug) {
        std::set<std::string>
    }

    inline bool isSuitable(vk::PhysicalDevice device, const bool debug) {
        if (debug) {
            std::cout << "Checking if device is suitable\n";
        }
        const std::vector<const char*> requestedExtensions {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };

        if (debug) {
            std::cout << "Requesting Device Extensions: \n";
            for (const char* extension : requestedExtensions) {
                std::cout << "\t\"" << extension << "\"\n";
            }
        }
        if (bool extensionSupported = checkDeviceExtensionSupport(device, requestedExtensions, debug)) {
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
            if (isSuitable(device, debug)) {
                return device;
            }
        }

        return nullptr;
    }
}

#endif //HYDROSIM_DEVICE_H