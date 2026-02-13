//
// Created by itari on 2026-01-24.
//

#ifndef HYDROSIM_ENGINE_H
#define HYDROSIM_ENGINE_H

#include "config.h"
#include "frame.h"

class Engine {
public:
    Engine(int width, int height, GLFWwindow* window, bool debug);
    ~Engine();
    void render();
private:
    const char* name = "Sim Window";
    bool debugMode;

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

    // Command Pool/Buffer Objects
    vk::CommandPool commandPool { nullptr };
    vk::CommandBuffer mainCommandBuffer { nullptr };

    // Synchronisation Objects
    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 3;
    uint32_t currentFrame = 0;

    std::array<vk::Semaphore, MAX_FRAMES_IN_FLIGHT> imageAvailable;
    std::array<vk::Fence, MAX_FRAMES_IN_FLIGHT> inFlightFences;

    // Track which fence is using each swapchain image
    std::vector<vk::Fence> imagesInFlight;

    void make_instance();

    void make_device();

    void make_pipeline();

    void finalize_setep();

    void record_draw_commands(const vk::CommandBuffer& commandBuffer, uint32_t imageIndex);
};


#endif //HYDROSIM_ENGINE_H