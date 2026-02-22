//
// Created by itari on 2026-02-21.
//
#include "memory.h"

namespace vkUtil {
    Buffer createBuffer(const BufferInput& input) {
        const auto bufferInfo = vk::BufferCreateInfo{}
        .setFlags(vk::BufferCreateFlags())
        .setSize(input.size)
        .setUsage(input.usage)
        .setSharingMode(vk::SharingMode::eExclusive);


        Buffer buffer;
        buffer.buffer = input.device.createBuffer(bufferInfo);

        allocateBufferMemory(buffer, input);
        return buffer;
    }

    uint32_t findMemoryTypeIndex(const vk::PhysicalDevice& physicalDevice, const uint32_t supportedMemoryIndices,
        const vk::MemoryPropertyFlags requestedProperties) {
        vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            const bool supported { static_cast<bool>(supportedMemoryIndices & 1 << i) };

            if (const bool sufficient {(memProperties.memoryTypes[i].propertyFlags & requestedProperties)
                == requestedProperties}; supported && sufficient) {
                return i;
                }
        }
        return 0;
    }
    void allocateBufferMemory(Buffer& buffer, const BufferInput& input) {
        const vk::MemoryRequirements memoryRequirements = input.device.getBufferMemoryRequirements(buffer.buffer);
        const auto allocateInfo = vk::MemoryAllocateInfo{}
        .setAllocationSize( memoryRequirements.size )
        .setMemoryTypeIndex(
            findMemoryTypeIndex(
                input.physicalDevice,
                memoryRequirements.memoryTypeBits,
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
            )
        );
        buffer.memory = input.device.allocateMemory(allocateInfo);
        input.device.bindBufferMemory(buffer.buffer, buffer.memory, 0);

    }
}