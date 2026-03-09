//
// Created by itari on 2026-02-25.
//

#ifndef HYDROSIM_VULKANENGINE_H
#define HYDROSIM_VULKANENGINE_H

#include "config.h"

class VulkanEngine {
public:
    explicit VulkanEngine(int window_width=640, int window_height=480, const std::string& engine_name="Vulkan Engine");

    ~VulkanEngine();

    void run() const {

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

private:

#ifdef NDEBUG
    static constexpr bool debug = false;
#else
    static constexpr bool debug = true;
#endif

    std::string name = "VulkanEngine";

    const std::vector<char const*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    std::vector<const char*> requiredDeviceExtension = {
        vk::KHRSwapchainExtensionName
    };

    int width { 640 };
    int height { 480 };
    GLFWwindow* window {nullptr};

    // Instance Objects
    vk::raii::Context context; // Vulkan Context
    vk::raii::Instance instance {nullptr}; // Vulkan Instance
    vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr; // Debug callback
    vk::detail::DispatchLoaderDynamic dldi; // Dynamic Instance dispatcher
    vk::SurfaceKHR surface {nullptr};

    // Device Objects
    vk::raii::PhysicalDevice physicalDevice { nullptr }; // Actual GPU
    vk::raii::Device device { nullptr }; // Abstraction of the GPU in use
    vk::raii::Queue graphicsQueue {nullptr};
    vk::raii::Queue presentQueue {nullptr};

    // Swapchain Objects
    vk::raii::SwapchainKHR swapchain { nullptr };
    // std::vector<vkUtil::SwapchainFrame> swapchainFrames;
    vk::Format swapchainFormat {};
    vk::Extent2D swapchainExtent {};

    [[nodiscard]] GLFWwindow* create_window() const;

    vk::raii::Instance create_instance();
    void create_debugger();
    void pick_physical_device();
    void create_logical_device();

};

#endif //HYDROSIM_VULKANENGINE_H
