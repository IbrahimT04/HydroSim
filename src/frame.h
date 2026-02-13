//
// Created by itari on 2026-02-07.
//

#ifndef HYDROSIM_FRAME_H
#define HYDROSIM_FRAME_H

#include "config.h"

namespace vkUtil {
    struct SwapchainFrame {
        vk::Image image;
        vk::ImageView imageView;
        vk::Framebuffer frameBuffer;
        vk::CommandBuffer commandBuffer;
        vk::Semaphore imageAvailable, renderFinished;
        vk::Fence inFlight;
    };
}

#endif //HYDROSIM_FRAME_H