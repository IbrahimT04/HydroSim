//
// Created by itari on 2026-01-24.
//

#ifndef HYDROSIM_ENGINE_H
#define HYDROSIM_ENGINE_H

#include "config.h"
#include "frame.h"
#include "scene.h"

class Engine {
public:
    Engine(int width, int height, GLFWwindow* window, bool debug);
    ~Engine();

    void render(const Scene* scene);

private:
    const char* name = "Sim Window";
    bool debugMode = false;

    int width { 640 };
    int height { 480 };
    GLFWwindow* window {nullptr};

    // Instance Objects
    vk::Instance instance {nullptr}; // Vulkan Instance
    vk::DebugUtilsMessengerEXT debugMessenger { nullptr }; // Debug callback
    vk::detail::DispatchLoaderDynamic dldi; // Dynamic Instance dispatcher
    vk::SurfaceKHR surface {nullptr};

    // Device Objects
    vk::PhysicalDevice physicalDevice { nullptr }; // Actual GPU
    vk::Device device { nullptr }; // Abstraction of the GPU in use
    vk::Queue graphicsQueue { nullptr };
    vk::Queue presentQueue { nullptr };

    // Swapchain Objects
    vk::SwapchainKHR swapchain { nullptr };
    std::vector<vkUtil::SwapchainFrame> swapchainFrames;
    vk::Format swapchainFormat {};
    vk::Extent2D swapchainExtent {};

    // Pipeline Objects
    vk::PipelineLayout pipelineLayout { nullptr };
    vk::RenderPass renderPass { nullptr };
    vk::Pipeline pipeline { nullptr };

    // Synchronisation Objects
    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 3;
    uint32_t currentFrame = 0;

    // Command Pool/Buffer Objects
    std::array<vkUtil::InFlightFrame, MAX_FRAMES_IN_FLIGHT> inFlightFrames;

    // Track which fence is using each swapchain image
    std::vector<vk::Fence> imagesInFlight;

    void make_instance();
    void make_device();

    void make_swapchain();
    void recreate_swapchain();
    void cleanup_swapchain();

    void make_pipeline();
    void cleanup_pipeline();

    void make_framebuffers();

    void create_inflight_frames();
    void destroy_inflight_frames();

    void record_draw_commands(const vk::CommandBuffer& commandBuffer, uint32_t imageIndex, const Scene* scene) const;
};


#endif //HYDROSIM_ENGINE_H