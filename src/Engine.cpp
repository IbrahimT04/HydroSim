//
// Created by itari on 2026-01-24.
//

#include "Engine.h"
#include <iostream>

#include "instance.h"

Engine::Engine() {
    if (debugMode) {
        std::cout << "Making a graphics Engine!\n";
    }
    build_glfw_window();
    make_instance();
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
}

void Engine::make_debug_message() {

    debugMessenger = vk::make_debug_message(instance, dldi);
}

Engine::~Engine() {
    if (debugMode) {
        std::cout << "Deleting the Engine!\n";
    }

    instance.destroy();

    glfwTerminate();
}
