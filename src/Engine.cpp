//
// Created by itari on 2026-01-24.
//

#include "Engine.h"
#include <iostream>

#include "queues.h"
#include "instance.h"
#include "logging.h"
#include "device.h"
#include "swapchain.h"

Engine::Engine() {
    if (debugMode) {
        std::cout << "Making a graphics Engine!\n";
    }
    build_glfw_window();
    make_instance();
    make_device();
}

void Engine::build_glfw_window() {

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    if ((window = glfwCreateWindow(width, height, name, nullptr, nullptr))) {
        if (debugMode) {
            std::cout << "Window Made Successfully\n";
        }
    }
    else {
        if (debugMode) {
            std::cout << "Window Making Failed\n";
        }
    }
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

Engine::~Engine() {
    if (debugMode) {
        std::cout << "Deleting the Engine!\n";
    }
    for (auto [image, imageView] : swapchainFrames) {
        device.destroyImageView(imageView);
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
