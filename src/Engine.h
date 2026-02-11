//
// Created by itari on 2026-01-24.
//

#ifndef HYDROSIM_ENGINE_H
#define HYDROSIM_ENGINE_H

#define DEBUG_MODE 1

#include "config.h"
#include "frame.h"

class Engine {
public:
    Engine();
    ~Engine();
private:
    static constexpr bool debugMode = DEBUG_MODE;
    const char* name = "Sim Window";

    int width { 640 };
    int height { 480 };
    GLFWwindow* window;

    // Instance Objects
    vk::Instance instance; // Vulkan Instance
    vk::DebugUtilsMessengerEXT debugMessenger { nullptr }; // Debug callback
    vk::detail::DispatchLoaderDynamic dldi; // Dynamic Instance dispatcher
    vk::SurfaceKHR surface;

    // Device Objects
    vk::PhysicalDevice physicalDevice { nullptr }; // Actual GPU
    vk::Device device { nullptr }; // Abstraction of the GPU in use
    vk::Queue graphicsQueue { nullptr };
    vk::Queue presentQueue { nullptr };
    vk::SwapchainKHR swapchain { nullptr };
    std::vector<vkUtil::SwapchainFrame> swapchainFrames;
    vk::Format swapchainFormat;
    vk::Extent2D swapchainExtent;

    // Pipeline Objects
    vk::PipelineLayout pipelineLayout { nullptr };
    vk::RenderPass renderPass { nullptr };
    vk::Pipeline pipeline { nullptr };

    void build_glfw_window();

    void make_instance();

    void make_device();

    void make_pipeline();
};


#endif //HYDROSIM_ENGINE_H