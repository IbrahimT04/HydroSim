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
    make_framebuffers();
    create_inflight_frames();

    make_assets();

    imagesInFlight.assign(swapchainFrames.size(), vk::Fence{});

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
        std::terminate();
    }
    if (debugMode) {
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

    make_swapchain();
}

void Engine::make_swapchain() {
    // vkInit::query_swapchain_support(physicalDevice, surface, debugMode);
    vkInit::SwapChainBundle bundle =
        vkInit::create_swapchain(device, physicalDevice, surface, width, height, debugMode);
    swapchain = bundle.swapchain;
    swapchainFrames = bundle.frames;
    swapchainFormat = bundle.format;
    swapchainExtent = bundle.extent;

    for (auto& frame : swapchainFrames) {
        frame.renderFinished = vkInit::make_semaphore(device, debugMode);
    }
}
void Engine::cleanup_swapchain() {
    for (const auto& frame : swapchainFrames) {
        if (frame.frameBuffer) device.destroyFramebuffer(frame.frameBuffer);
        if (frame.imageView) device.destroyImageView(frame.imageView);

        if (frame.renderFinished) device.destroySemaphore(frame.renderFinished);
    }

    swapchainFrames.clear();
    imagesInFlight.clear();

    if (swapchain) {
        device.destroySwapchainKHR(swapchain);
        swapchain = nullptr;
    }
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
void Engine::cleanup_pipeline() {
    if (pipeline) {
        device.destroyPipeline(pipeline);
        pipeline = nullptr;
    }
    if (renderPass) {
        device.destroyRenderPass(renderPass);
        renderPass = nullptr;
    }
    if (pipelineLayout) {
        device.destroyPipelineLayout(pipelineLayout);
        pipelineLayout = nullptr;
    }

}

void Engine::make_framebuffers() {
    vkInit::FrameBufferInput frameBufferInput;
    frameBufferInput.device = device;
    frameBufferInput.renderPass = renderPass;
    frameBufferInput.swapchainExtent = swapchainExtent;
    vkInit::make_framebuffer(frameBufferInput, swapchainFrames, debugMode);
}

void Engine::create_inflight_frames() {
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        auto& frame = inFlightFrames[i];
        frame.commandPool = vkInit::make_command_pool(device, physicalDevice, surface, debugMode);
        frame.mainCommandBuffer = vkInit::make_command_buffer(device, frame.commandPool, debugMode);

        frame.imageAvailable = vkInit::make_semaphore(device, debugMode);

        frame.fence = vkInit::make_fence(device, debugMode);

        frame.imageIndex = 0;
    }
}
void Engine::destroy_inflight_frames() {
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        auto& frame = inFlightFrames[i];

        if (frame.fence) device.destroyFence(frame.fence);
        if (frame.imageAvailable) device.destroySemaphore(frame.imageAvailable);
        if (frame.commandPool) device.destroyCommandPool(frame.commandPool);

        frame = {};
    }
}

void Engine::recreate_swapchain() {

    // If window is minimized then extent can be 0 so just skip swapchain recreation until it becomes valid.
    int fbW = 0, fbH = 0;
    glfwGetFramebufferSize(window, &fbW, &fbH);
    if (fbW == 0 || fbH == 0) {
        return;
    }

    device.waitIdle();

    // Swapchain related things cleaned up.
    cleanup_swapchain();
    cleanup_pipeline();

    width = fbW;
    height = fbH;

    // Remake swapchain
    make_swapchain();
    make_pipeline();
    make_framebuffers();

    imagesInFlight.assign(swapchainFrames.size(), vk::Fence{});
}

void Engine::make_assets() {
    triangleMesh = new vkMesh::TriangleMesh(device, physicalDevice);
}
void Engine::prepare_scene(const vk::CommandBuffer& commandBuffer) const {
    const vk::Buffer vertexBuffers[] = { triangleMesh ->vertexBuffer.buffer };
    const vk::DeviceSize offsets[] = { 0 };
    commandBuffer.bindVertexBuffers(0,  1, vertexBuffers, offsets);
}

void Engine::record_draw_commands(const vk::CommandBuffer& commandBuffer, const uint32_t imageIndex, const Scene* scene) const {

    commandBuffer.begin(vk::CommandBufferBeginInfo{});

    std::array<vk::ClearValue, 1> clearValues{};
    clearValues[0].color = vk::ClearColorValue(std::array<float,4>{ 1.0f, 0.5f, 0.25f, 1.0f });

    vk::RenderPassBeginInfo renderInfo{};
    renderInfo.renderPass = renderPass;
    renderInfo.framebuffer = swapchainFrames[imageIndex].frameBuffer;
    renderInfo.renderArea = vk::Rect2D({0, 0}, swapchainExtent);
    renderInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderInfo.pClearValues = clearValues.data();

    commandBuffer.beginRenderPass(renderInfo, vk::SubpassContents::eInline);
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

    prepare_scene(commandBuffer);

    for (glm::vec3 position : scene->trianglePositions) {
        const glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        vkUtil::ObjectData objectData{};
        objectData.model = model;
        commandBuffer.pushConstants(pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(objectData), &objectData);
        commandBuffer.draw(3, 1, 0, 0);
    }
    commandBuffer.endRenderPass();

    commandBuffer.end();
}

void Engine::render(const Scene* scene) {

    // if window is minimized then do not render this frame
    int fbW = 0, fbH = 0;
    glfwGetFramebufferSize(window, &fbW, &fbH);
    if (fbW == 0 || fbH == 0) {
        return;
    }

    // Step 0: Get current in-flight frame
    auto& inFlightFrame = inFlightFrames[currentFrame];


    // Step 1: Wait for CPU-GPU sync for this in-flight frame
    VK_CHECK(device.waitForFences(1, &inFlightFrame.fence, VK_TRUE, UINT64_MAX));


    // Step 2: Acquire image index
    const auto acquire =
        device.acquireNextImageKHR(swapchain, UINT64_MAX, inFlightFrame.imageAvailable, nullptr);

    if (acquire.result == vk::Result::eErrorOutOfDateKHR) {
        recreate_swapchain();
        return;
    }
    if (acquire.result != vk::Result::eSuccess && acquire.result != vk::Result::eSuboptimalKHR) {
        std::terminate();
    }
    const uint32_t imageIndex = acquire.value;
    inFlightFrame.imageIndex = imageIndex;


    // Step 3: Wait for swapchain image if it is in-flight
    if (imagesInFlight[imageIndex]) {
        VK_CHECK(device.waitForFences(1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX));
    }
    imagesInFlight[imageIndex] = inFlightFrame.fence;


    // Step 4: Reset fence for this in-flight frame
    VK_CHECK(device.resetFences(1, &inFlightFrame.fence));


    // Step 5: Record commands for current swapchain image
    device.resetCommandPool(inFlightFrame.commandPool); // Reset the command pool not the buffer for efficiency

    record_draw_commands(inFlightFrame.mainCommandBuffer, imageIndex, scene);


    // Step 6: Submit image to graphics queue
    const vk::Semaphore waitSem = inFlightFrame.imageAvailable;
    const vk::Semaphore signalSem = swapchainFrames[imageIndex].renderFinished;

    constexpr vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    vk::SubmitInfo submitInfo{};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &waitSem;
    submitInfo.pWaitDstStageMask = &waitStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &inFlightFrame.mainCommandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &signalSem;

    graphicsQueue.submit(submitInfo, inFlightFrame.fence);


    // Step 7: Present image
    vk::PresentInfoKHR presentInfo{};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &signalSem;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &imageIndex;

    try {
        const vk::Result presentResult = presentQueue.presentKHR(presentInfo);

        if (presentResult == vk::Result::eErrorOutOfDateKHR ||
            presentResult == vk::Result::eSuboptimalKHR) {
            recreate_swapchain();
            return;
            }
        if (presentResult != vk::Result::eSuccess) {
            std::terminate();
        }
    }
    catch (const vk::OutOfDateKHRError&) {
        recreate_swapchain();
        return;
    }
    catch (const vk::SystemError&) {
        std::terminate();
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}


Engine::~Engine() {

    device.waitIdle();

    if (debugMode) {
        std::cout << "Deleting the Engine!\n";
    }
    destroy_inflight_frames();
    cleanup_pipeline();
    cleanup_swapchain();

    delete triangleMesh;

    if (device) device.destroy();
    if (surface) instance.destroySurfaceKHR(surface);
    if (debugMode && debugMessenger) {
        instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
    }
    if (instance) instance.destroy();

    glfwTerminate();
}
