//
// Created by itari on 2026-01-31.
//

#ifndef HYDROSIM_QUEUES_H
#define HYDROSIM_QUEUES_H
#include <cstdint>
#include <optional>
#include "config.h"

namespace vkInit {
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() const {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    inline QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device, const vk::SurfaceKHR surface, const bool debug) {
        QueueFamilyIndices indices;

        std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

        if (debug) {
            std::cout << "System can support " << queueFamilies.size() << " queue families.\n";
        }
        int i = 0;
        for (vk::QueueFamilyProperties queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
                indices.graphicsFamily = i;

                if (debug) {
                    std::cout << "Queue family " << i << " is suitable for graphics.\n";
                }
            }

            if (device.getSurfaceSupportKHR(i, surface)) {
                indices.presentFamily = i;

                if (debug) {
                    std::cout << "Queue family " << i << " is suitable for presenting.\n";
                }
            }

            if (indices.isComplete()) {
                break;
            }

            i++;
        }


        return indices;
    }

    inline std::array<vk::Queue, 2> get_queue(const vk::PhysicalDevice physicalDevice, const vk::Device device, const vk::SurfaceKHR surface, const bool debug) {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface, debug);

        return {
                device.getQueue(indices.graphicsFamily.value(), 0),

                device.getQueue(indices.presentFamily.value(), 0)
            };
    }
}

#endif //HYDROSIM_QUEUES_H