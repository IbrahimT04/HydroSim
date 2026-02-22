//
// Created by itari on 2026-02-16.
//

#ifndef HYDROSIM_MEMORY_H
#define HYDROSIM_MEMORY_H

#include "config.h"

namespace vkUtil {

    struct BufferInput {
        size_t size;
        vk::BufferUsageFlags usage;
        vk::Device device;
        vk::PhysicalDevice physicalDevice;
    };

    struct Buffer {
        vk::Buffer buffer;
        vk::DeviceMemory memory;
    };

    Buffer createBuffer(const BufferInput& input);

    uint32_t findMemoryTypeIndex(const vk::PhysicalDevice& physicalDevice, const uint32_t supportedMemoryIndices,
        const vk::MemoryPropertyFlags requestedProperties);

    void allocateBufferMemory(Buffer& buffer, const BufferInput& input);

}

#endif //HYDROSIM_MEMORY_H