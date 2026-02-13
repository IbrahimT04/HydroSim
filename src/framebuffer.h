//
// Created by itari on 2026-02-12.
//

#ifndef HYDROSIM_FRAMEBUFFER_H
#define HYDROSIM_FRAMEBUFFER_H

#include "config.h"
#include "frame.h"

namespace vkInit {

    struct FrameBufferInput {
        vk::Device device;
        vk::RenderPass renderPass;
        vk::Extent2D swapchainExtent;
    };

    inline void make_framebuffer(const FrameBufferInput& inputChunk, std::vector<vkUtil::SwapchainFrame>& frames, const bool debug) {
        for (int i = 0; i < frames.size(); i++) {
            std::vector<vk::ImageView> attachments = {
                frames[i].imageView
            };

            vk::FramebufferCreateInfo frameBufferInfo = {};
            frameBufferInfo.flags = vk::FramebufferCreateFlags();
            frameBufferInfo.renderPass = inputChunk.renderPass;
            frameBufferInfo.attachmentCount = attachments.size();
            frameBufferInfo.pAttachments = attachments.data();
            frameBufferInfo.width = inputChunk.swapchainExtent.width;
            frameBufferInfo.height = inputChunk.swapchainExtent.height;
            frameBufferInfo.layers = 1;

            try {
                frames[i].frameBuffer = inputChunk.device.createFramebuffer(frameBufferInfo);
                if (debug) {
                    std::cout << "Framebuffer created for frame " << i << std::endl;
                }
            }
            catch (vk::SystemError&) {
                if (debug) {
                    std::cout << "Failed to create framebuffer for frame "<< i << std::endl;
                }
            }
        }

    }

}

#endif //HYDROSIM_FRAMEBUFFER_H