//
// Created by itari on 2026-01-24.
//

#include "Engine.h"

#include <iostream>

#include "commands.h"
#include "queues.h"
#include "instance.h"
#include "logging.h"
#include "device.h"
#include "pipeline.h"
#include "swapchain.h"
#include "framebuffer.h"
#include "sync.h"

#define VK_CHECK(x) do { vk::Result r_ = (x); if (r_ != vk::Result::eSuccess) std::terminate(); } while(0)

Engine::Engine(const int width, const int height, GLFWwindow* window, const bool debug) {

    this->width = width;
    this->height = height;
    this->window = window;
    this->debugMode = debug;

    if (debugMode) {
        std::cout << "Making a graphics Engine!\n";
    }
    make_instance();
    make_device();
    make_pipeline();
    finalize_setep();
}

void Engine::make_instance() {
    instance = vkInit::make_instance(debugMode, name);

    dldi = vk::detail::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);

    if (debugMode) {
        debugMessenger = vkInit::make_debug_messeger(instance, dldi);
    }
    VkSurfaceKHR cStyleSurface;
    if (glfwCreateWindowSurface(instance, window, nullptr, &cStyleSurface) != VK_SUCCESS) {
        if (debugMode) {
            std::cout << "Window Create Failed for glfw with Vulkan.\n";
        }
    }
    else if (debugMode) {
        std::cout << "Window Create Successfully for glfw with Vulkan.\n";
    }
    surface = cStyleSurface;
}

void Engine::make_device() {
    physicalDevice = vkInit::choose_physical_device(instance, debugMode);
    device = vkInit::create_logical_device(physicalDevice, surface, debugMode);
    const std::array<vk::Queue, 2> queues = vkInit::get_queue(physicalDevice, device, surface, debugMode);
    graphicsQueue = queues[0];
    presentQueue = queues[1];

    // vkInit::query_swapchain_support(physicalDevice, surface, debugMode);
    vkInit::SwapChainBundle bundle = vkInit::create_swapchain(device, physicalDevice, surface, width, height, debugMode);
    swapchain = bundle.swapchain;
    swapchainFrames = bundle.frames;
    swapchainFormat = bundle.format;
    swapchainExtent = bundle.extent;
}

void Engine::make_pipeline() {
    vkInit::GraphicsPipelineInBundle specification = {};
    specification.device = device;
    specification.vertexFilePath = "spir_v_shaders/default.vert.spv";
    specification.fragmentFilePath = "spir_v_shaders/default.frag.spv";
    specification.swapchainExtent = swapchainExtent;
    specification.swapchainFormat = swapchainFormat;
    vkInit::GraphicsPipelineOutBundle output = vkInit::make_graphics_pipeline(specification, debugMode);
    pipelineLayout = output.pipelineLayout;
    renderPass = output.renderPass;
    pipeline = output.pipeline;
}
void Engine::finalize_setep() {
    vkInit::FrameBufferInput frameBufferInput;
    frameBufferInput.device = device;
    frameBufferInput.renderPass = renderPass;
    frameBufferInput.swapchainExtent = swapchainExtent;
    vkInit::make_framebuffer(frameBufferInput, swapchainFrames, debugMode);

    commandPool = vkInit::make_command_pool(device, physicalDevice, surface, debugMode);
    vkInit::CommandBufferInputChunk inputChunk = { device, commandPool, swapchainFrames};
    mainCommandBuffer = vkInit::make_command_buffers(inputChunk, debugMode);

    imagesInFlight.assign(swapchainFrames.size(), vk::Fence{});

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        imageAvailable[i]  = vkInit::make_semaphore(device, debugMode);
        inFlightFences[i]  = vkInit::make_fence(device, debugMode);
    }

    for (auto& frame : swapchainFrames) {
        frame.renderFinished = vkInit::make_semaphore(device, debugMode);
    }
}

void Engine::record_draw_commands(const vk::CommandBuffer& commandBuffer, const uint32_t imageIndex) {

    commandBuffer.begin(vk::CommandBufferBeginInfo{});

    std::array<vk::ClearValue, 1> clearValues{};
    clearValues[0].color = vk::ClearColorValue(std::array<float,4>{ 1.0f, 0.5f, 0.25f, 1.0f });

    vk::RenderPassBeginInfo rpBegin{};
    rpBegin.renderPass = renderPass;
    rpBegin.framebuffer = swapchainFrames[imageIndex].frameBuffer;
    rpBegin.renderArea = vk::Rect2D({0, 0}, swapchainExtent);
    rpBegin.clearValueCount = static_cast<uint32_t>(clearValues.size());
    rpBegin.pClearValues = clearValues.data();

    commandBuffer.beginRenderPass(rpBegin, vk::SubpassContents::eInline);
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
    commandBuffer.draw(3, 1, 0, 0);
    commandBuffer.endRenderPass();

    commandBuffer.end();
}

void Engine::render() {

    // Step 1: Wait for CPU-GPU sync for this in-flight frame
    VK_CHECK(device.waitForFences(1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX));

    // Step 2: Acquire image index
    uint32_t imageIndex =
        device.acquireNextImageKHR(swapchain, UINT64_MAX, imageAvailable[currentFrame], nullptr).value;

    // Step 3: Wait for swapchain image if it is in-flight
    if (imagesInFlight[imageIndex]) {
        VK_CHECK(device.waitForFences(1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX));
    }
    imagesInFlight[imageIndex] = inFlightFences[currentFrame];

    // Step 4: Reset fence for this in-flight frame
    VK_CHECK(device.resetFences(1, &inFlightFences[currentFrame]));

    // Step 5: Record commands for current swapchain image
    vk::CommandBuffer commandBuffer = swapchainFrames[imageIndex].commandBuffer;
    commandBuffer.reset();
    record_draw_commands(commandBuffer, imageIndex);

    // Step 6: Submit image to graphics queue
    vk::Semaphore waitSem = imageAvailable[currentFrame];
    vk::Semaphore signalSem = swapchainFrames[imageIndex].renderFinished;

    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    vk::SubmitInfo submitInfo{};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &waitSem;
    submitInfo.pWaitDstStageMask = &waitStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &signalSem;

    graphicsQueue.submit(submitInfo, inFlightFences[currentFrame]);

    // Step 7: Present image
    vk::PresentInfoKHR presentInfo{};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &signalSem;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &imageIndex;

    VK_CHECK(presentQueue.presentKHR(presentInfo));

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}


Engine::~Engine() {

    device.waitIdle();

    if (debugMode) {
        std::cout << "Deleting the Engine!\n";
    }
    device.destroyCommandPool(commandPool);
    device.destroyPipeline(pipeline);
    device.destroyRenderPass(renderPass);
    device.destroyPipelineLayout(pipelineLayout);

    for (const auto frame : swapchainFrames) {
        device.destroySemaphore(frame.renderFinished);
        device.destroyImageView(frame.imageView);
        device.destroyFramebuffer(frame.frameBuffer);
    }
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        device.destroySemaphore(imageAvailable[i]);
        device.destroyFence(inFlightFences[i]);
    }
    device.destroySwapchainKHR(swapchain);
    device.destroy();
    instance.destroySurfaceKHR(surface);
    if (debugMode) {
        instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
    }
    instance.destroy();
    glfwTerminate();

}
