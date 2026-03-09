//
// Created by itari on 2026-02-07.
//

#ifndef HYDROSIM_FRAME_H
#define HYDROSIM_FRAME_H

#include "config.h"

namespace vkUtil {
    struct SwapchainFrame {
        vk::Image image {nullptr};
        vk::ImageView imageView {nullptr};
        vk::Framebuffer frameBuffer {nullptr};

        vk::Semaphore renderFinished {nullptr};
    };

    struct InFlightFrame {
        vk::CommandPool commandPool {nullptr};
        vk::CommandBuffer mainCommandBuffer {nullptr};

        vk::Semaphore imageAvailable {nullptr};
        vk::Fence fence {nullptr};

        uint32_t imageIndex = 0;
    };
}

#endif //HYDROSIM_FRAME_H