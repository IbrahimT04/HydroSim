//
// Created by itari on 2026-02-25.
//

#ifndef HYDROSIM_VULKANENGINE_H
#define HYDROSIM_VULKANENGINE_H

#include "config.h"
#include "buffer_helpers.h"

class VulkanEngine {
public:

    explicit VulkanEngine(int window_width = 640, int window_height = 480,
                          const std::string &engine_name = "Vulkan Engine");

    ~VulkanEngine();

    void run() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            drawFrame();
        }
        device.waitIdle();
    }

    void drawFrame() {

        update_uniform_buffer(frameIndex);

        if (const auto fenceResult = device.waitForFences(*inFlightFences[frameIndex], vk::True, UINT64_MAX);
            fenceResult != vk::Result::eSuccess) {
            throw std::runtime_error("failed to wait for fence!");
        }

        auto [result, imageIndex] = swapchain.acquireNextImage(
            UINT64_MAX, *presentCompleteSemaphores[frameIndex], nullptr);

        if (result == vk::Result::eErrorOutOfDateKHR) {
            recreate_swapchain();
            return;
        }
        if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
            assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        device.resetFences(*inFlightFences[frameIndex]);

        commandBuffers[frameIndex].reset();
        record_command_buffer(imageIndex);

        vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        const vk::SubmitInfo submitInfo{
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &*presentCompleteSemaphores[frameIndex],
            .pWaitDstStageMask = &waitDestinationStageMask,
            .commandBufferCount = 1,
            .pCommandBuffers = &*commandBuffers[frameIndex],
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &*renderFinishedSemaphores[imageIndex]
        };
        queue.submit(submitInfo, *inFlightFences[frameIndex]);

        const vk::PresentInfoKHR presentInfoKHR{
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &*renderFinishedSemaphores[imageIndex],
            .swapchainCount = 1,
            .pSwapchains = &*swapchain,
            .pImageIndices = &imageIndex
        };

        result = queue.presentKHR(presentInfoKHR);

        if ((result == vk::Result::eSuboptimalKHR) || (result == vk::Result::eErrorOutOfDateKHR) ||
            framebufferResized) {
            framebufferResized = false;
            recreate_swapchain();
        } else {
            // There are no other success codes than eSuccess; on any error code, presentKHR already threw an exception.
            assert(result == vk::Result::eSuccess);
        }
        frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    }

private:
#ifdef NDEBUG
    static constexpr bool debug = false;
#else
    static constexpr bool debug = true;
#endif
    static constexpr bool indexing = true;

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
    std::vector<vk::raii::ImageView> swapchainImageViews;

    // Pipeline Objects
    vk::raii::PipelineLayout pipelineLayout{VK_NULL_HANDLE};
    vk::raii::Pipeline graphicsPipeline{VK_NULL_HANDLE};

    // Command Objects
    vk::raii::CommandPool commandPool{VK_NULL_HANDLE};
    std::vector<vk::raii::CommandBuffer> commandBuffers;

    // Synchronisation Objects
    std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
    std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
    std::vector<vk::raii::Fence> inFlightFences;

    // Vertex Objects
    vk::raii::Buffer vertexBuffer{VK_NULL_HANDLE};
    vk::raii::DeviceMemory vertexBufferMemory{VK_NULL_HANDLE};
    vk::raii::Buffer indexBuffer{VK_NULL_HANDLE};
    vk::raii::DeviceMemory indexBufferMemory{VK_NULL_HANDLE};

    // Descriptor Objects
    vk::raii::DescriptorSetLayout descriptorSetLayout{VK_NULL_HANDLE};

    // Uniform Buffers
    std::vector<vk::raii::Buffer> uniformBuffers;
    std::vector<vk::raii::DeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    // Data Objects
    const std::vector<vkBuffer::Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };
    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    };

    // Frame Objects
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t frameIndex{0};
    bool framebufferResized = false;

    void create_window();

    static void framebuffer_resize_callback(GLFWwindow *window, int width, int height);

    vk::raii::Instance create_instance();

    void create_debugger();

    void pick_physical_device();

    void create_logical_device();

    void create_surface();

    void create_swapchain();

    void create_image_views();

    [[nodiscard]] vk::raii::ShaderModule create_shader_module(const std::vector<char> &code) const;

    void create_descriptor_set_layout();

    void create_graphics_pipeline();

    void create_command_pool();

    void create_vertex_buffer();

    void create_index_buffer();

    void create_uniform_buffers();

    void create_buffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties,
                       vk::raii::Buffer &buffer, vk::raii::DeviceMemory &bufferMemory) const;

    [[nodiscard]] uint32_t find_memory_type(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;

    void copyBuffer(const vk::raii::Buffer &srcBuffer, const vk::raii::Buffer &dstBuffer, vk::DeviceSize size) const;

    void create_command_buffers();

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

    void cleanup_swapchain();

    void recreate_swapchain();

    void update_uniform_buffer(uint32_t currentImage);
};

#endif //HYDROSIM_VULKANENGINE_H
