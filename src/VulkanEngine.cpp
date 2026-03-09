//
// Created by itari on 2026-02-25.
//
#include "VulkanEngine.h"

#include "instance_helpers.h"
#include "logging.h"
#include "device_helpers.h"

VulkanEngine::VulkanEngine(const int window_width, const int window_height, const std::string& engine_name) {

    name = engine_name;
    width = window_width;
    height = window_height;

    window = create_window();
    instance = create_instance();
    create_debugger();
    create_surface();
    pick_physical_device();
    create_logical_device();
    queue = vk::raii::Queue(device,vkDevice::get_queue_index(physicalDevice,surface),0);

}
GLFWwindow* VulkanEngine::create_window() const {

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    return glfwCreateWindow(width, height, "Vulkan Window", nullptr, nullptr);
}

vk::raii::Instance VulkanEngine::create_instance() {

    // Setup Application Info.
    constexpr vk::ApplicationInfo app_info {
        .pApplicationName = "Vulkan Engine Application",
        .applicationVersion = VK_MAKE_VERSION( 1, 4, 0 ),
        .pEngineName = "Basic Engine",
        .engineVersion = VK_MAKE_VERSION( 1, 4, 0 ),
        .apiVersion = vk::ApiVersion14
    };

    // -- EXTENSIONS --
    if constexpr(debug) vkInit::print_available_extensions(context);
    const auto requiredExtensions = vkInit::getRequiredInstanceExtensions(debug);
    vkInit::extensions_supported(context, requiredExtensions);

    // -- VALIDATION LAYERS --
    if constexpr(debug) vkInit::print_available_validation_layers(context);
    std::vector<char const*> requiredLayers;
    // Get the required layers
    if constexpr(debug) requiredLayers.assign(validationLayers.begin(), validationLayers.end());
    vkInit::validation_layers_supported(context, requiredLayers);


    // Setup information for creating a Vulkan Instance
    const vk::InstanceCreateInfo create_info {
        .pApplicationInfo = &app_info,
        .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
        .ppEnabledLayerNames = requiredLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
        .ppEnabledExtensionNames = requiredExtensions.data()
    };

    return {context, create_info};
}

void VulkanEngine::create_debugger() {

    debugMessenger = vkLogging::setupDebugMesssenger(instance);
}

void VulkanEngine::pick_physical_device() {

    physicalDevice = vkDevice::get_most_suitable_physical_device(instance, requiredDeviceExtension);
}
void VulkanEngine::create_logical_device() {

    device = vkDevice::make_logical_device(physicalDevice, surface, requiredDeviceExtension);
}
void VulkanEngine::create_surface() {
    VkSurfaceKHR       _surface;
    if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != 0) {
        throw std::runtime_error("failed to create window surface!");
    }
    surface = vk::raii::SurfaceKHR(instance, _surface);
}

VulkanEngine::~VulkanEngine() {
    glfwDestroyWindow(window);

    glfwTerminate();
}
