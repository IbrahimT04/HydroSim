//
// Created by itari on 2026-01-24.
//

#include "Engine.h"
#include <iostream>

Engine::Engine() {
    if (debugMode) {
        std::cout << "Making a graphics Engine!\n";
    }
    build_glfw_window();
}

void Engine::build_glfw_window() {

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    if ((window = glfwCreateWindow(width, height, "Sim Window", nullptr, nullptr))) {
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
Engine::~Engine() {
    if (debugMode) {
        std::cout << "Deleting the Engine!\n";
    }
    glfwTerminate();
}
