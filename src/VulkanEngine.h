//
// Created by itari on 2026-02-25.
//

#ifndef HYDROSIM_VULKANENGINE_H
#define HYDROSIM_VULKANENGINE_H

#include "config.h"

class VulkanEngine {
public:
    explicit VulkanEngine(int window_width = 640, int window_height = 480,
                          const std::string &engine_name = "Vulkan Engine");

    ~VulkanEngine();

    void run() const {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            drawFrame();
        }
        device.waitIdle();
    }

    void drawFrame() const {
        auto fenceResult = device.waitForFences(*drawFence, vk::True, UINT64_MAX);
        auto [result, imageIndex] = swapchain.acquireNextImage(
            UINT64_MAX,
            *presentCompleteSemaphore,
            nullptr);
        record_command_buffer(imageIndex);
        device.resetFences(*drawFence);

        vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        const vk::SubmitInfo submitInfo{
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &*presentCompleteSemaphore,
            .pWaitDstStageMask = &waitDestinationStageMask,
            .commandBufferCount = 1,
            .pCommandBuffers = &*commandBuffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &*renderFinishedSemaphore
        };

        queue.submit(submitInfo, *drawFence);


        const vk::PresentInfoKHR presentInfoKHR{
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &*renderFinishedSemaphore,
            .swapchainCount = 1,
            .pSwapchains = &*swapchain,
            .pImageIndices = &imageIndex
        };

        result = queue.presentKHR(presentInfoKHR);
    }

private:
#ifdef NDEBUG
    static constexpr bool debug = false;
#else
    static constexpr bool debug = true;
#endif

    std::string name = "VulkanEngine";

    const std::vector<char const *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    std::vector<const char *> requiredDeviceExtension = {
        vk::KHRSwapchainExtensionName
    };

    int width{640};
    int height{480};
    GLFWwindow *window{VK_NULL_HANDLE};

    // Instance Objects
    vk::raii::Context context; // Vulkan Context
    vk::raii::Instance instance{VK_NULL_HANDLE}; // Vulkan Instance
    vk::raii::DebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE; // Debug callback
    vk::detail::DispatchLoaderDynamic dldi; // Dynamic Instance dispatcher
    vk::raii::SurfaceKHR surface{VK_NULL_HANDLE};

    // Device Objects
    vk::raii::PhysicalDevice physicalDevice{VK_NULL_HANDLE}; // Actual GPU
    vk::raii::Device device{VK_NULL_HANDLE}; // Abstraction of the GPU in use
    vk::raii::Queue queue{VK_NULL_HANDLE};

    // Swapchain Objects
    vk::raii::SwapchainKHR swapchain{VK_NULL_HANDLE};
    std::vector<vk::Image> swapchainImages;
    vk::SurfaceFormatKHR swapchainSurfaceFormat{};
    vk::Extent2D swapchainExtent{};
    std::vector<vk::raii::ImageView> swapChainImageViews;

    // Pipeline Objects
    vk::raii::PipelineLayout pipelineLayout{VK_NULL_HANDLE};
    vk::raii::Pipeline graphicsPipeline{VK_NULL_HANDLE};

    // Command Objects
    vk::raii::CommandPool commandPool{VK_NULL_HANDLE};
    vk::raii::CommandBuffer commandBuffer{VK_NULL_HANDLE};

    // Synchronisation
    vk::raii::Semaphore presentCompleteSemaphore{VK_NULL_HANDLE};
    vk::raii::Semaphore renderFinishedSemaphore{VK_NULL_HANDLE};
    vk::raii::Fence drawFence{VK_NULL_HANDLE};

    [[nodiscard]] GLFWwindow *create_window() const;

    vk::raii::Instance create_instance();

    void create_debugger();

    void pick_physical_device();

    void create_logical_device();

    void create_surface();

    void create_swapchain();

    void create_image_views();

    [[nodiscard]] vk::raii::ShaderModule create_shader_module(const std::vector<char> &code) const;

    void create_graphics_pipeline();

    void create_command_pool();

    void create_command_buffer();

    void create_synchronization_objects();

    void transition_image_layout(
        uint32_t imageIndex,
        vk::ImageLayout oldLayout,
        vk::ImageLayout newLayout,
        vk::AccessFlags2 srcAccessMask,
        vk::AccessFlags2 dstAccessMask,
        vk::PipelineStageFlags2 srcStageMask,
        vk::PipelineStageFlags2 dstStageMask) const;

    void record_command_buffer(uint32_t imageIndex) const;
};

#endif //HYDROSIM_VULKANENGINE_H
