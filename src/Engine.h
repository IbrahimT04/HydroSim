//
// Created by itari on 2026-01-24.
//

#ifndef HYDROSIM_ENGINE_H
#define HYDROSIM_ENGINE_H
#include "GLFW/glfw3.h"
#include "config.h"


class Engine {
public:
    Engine();
    ~Engine();
private:
    bool debugMode = true;
    const char* name = "Sim Window";

    int width { 640 };
    int height { 480 };
    GLFWwindow* window;

    // Instance Objects
    vk::Instance instance; // Vulkan Instance
    vk::DebugUtilsMessengerEXT debugMessenger { nullptr }; // Debug callback
    vk::detail::DispatchLoaderDynamic dldi; // Dynamic Instance dispatcher

    vk::PhysicalDevice physicalDevice { nullptr };

    void build_glfw_window();

    void make_instance();

    void make_device();
};


#endif //HYDROSIM_ENGINE_H