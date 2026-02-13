//
// Created by itari on 2026-02-12.
//

#ifndef HYDROSIM_COMMANDS_H
#define HYDROSIM_COMMANDS_H

#include "config.h"
#include "queues.h"

namespace vkInit {

    struct CommandBufferInputChunk {
        vk::Device device;
        vk::CommandPool commandPool;
        std::vector<vkUtil::SwapchainFrame>& frames;
    };

    inline vk::CommandPool make_command_pool(const vk::Device& device, const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, const bool debug) {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice, surface, debug);

        vk::CommandPoolCreateInfo commandPoolCreateInfo = {};
        commandPoolCreateInfo.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        try {
            return device.createCommandPool(commandPoolCreateInfo);
        }
        catch (vk::SystemError&) {
            if (debug) {
                std::cerr << "Failed to create command pool" << std::endl;
            }
            return nullptr;
        }
    }
    inline vk::CommandBuffer make_command_buffers(const CommandBufferInputChunk& inputChunk, const bool debug) {
        vk::CommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.commandPool = inputChunk.commandPool;
        allocateInfo.level = vk::CommandBufferLevel::ePrimary;
        allocateInfo.commandBufferCount = 1;

        for (int i=0; i < inputChunk.frames.size(); i++) {
            try {
                inputChunk.frames[i].commandBuffer = inputChunk.device.allocateCommandBuffers(allocateInfo)[0];
                if (debug) {
                    std::cout << "Allocated command buffer for frame " << i << std::endl;
                }
            }
            catch (vk::SystemError&) {
                if (debug) {
                    std::cerr << "Failed to allocate command buffer for frame" << i << std::endl;
                }
            }
        }

        try {
            vk::CommandBuffer mainCommandBuffer = inputChunk.device.allocateCommandBuffers(allocateInfo)[0];
            if (debug) {
                std::cout << "Allocated main command buffer" << std::endl;
            }
            return mainCommandBuffer;
        }
        catch (vk::SystemError&) {
            if (debug) {
                std::cerr << "Failed to allocate main command buffer" << std::endl;
            }
            return nullptr;
        }
    }

}

#endif //HYDROSIM_COMMANDS_H