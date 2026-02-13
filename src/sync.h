//
// Created by itari on 2026-02-12.
//

#ifndef HYDROSIM_SYNC_H
#define HYDROSIM_SYNC_H

#include "config.h"

namespace vkInit {
    inline vk::Semaphore make_semaphore(const vk::Device& device, const bool debug){
        vk::SemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.flags = vk::SemaphoreCreateFlags();

        try {
            return device.createSemaphore(semaphoreInfo);
        }
        catch (vk::SystemError&) {
            if (debug) {
                std::cerr << "Failed to create semaphore!" << std::endl;
            }
            return nullptr;
        }
    }
    inline vk::Fence make_fence(const vk::Device& device, const bool debug) {
        vk::FenceCreateInfo fenceInfo = {};
        fenceInfo.flags = vk::FenceCreateFlags() | vk::FenceCreateFlagBits::eSignaled;

        try {
            return device.createFence(fenceInfo);
        }
        catch (vk::SystemError&) {
            if (debug) {
                std::cerr << "Failed to create fence!" << std::endl;
            }
            return nullptr;
        }
    }
}

#endif //HYDROSIM_SYNC_H