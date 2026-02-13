//
// Created by itari on 2026-02-12.
//
#include "app.h"

#include <sstream>

#include "Engine.h"

App::App(const std::string &name, const int width, const int height) {
    windowName = name;
    build_glfw_window(width, height);

    graphicsEngine = new Engine(width, height, window, debugMode);
}

void App::build_glfw_window(const int width, const int height) {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    if ((window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr))) {
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

void App::run() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        graphicsEngine->render();
        calculate_frame_rate();
    }
}

void App::calculate_frame_rate() {
    currentTime = glfwGetTime();
    if (const double deltaTime = currentTime - lastTime; deltaTime >= 1.0) {
        const int fps{ std::max(1, static_cast<int>(numFrames / deltaTime)) };
        std::stringstream title;
        title << "Running " << windowName << " at FPS: " << fps << " fps";
        glfwSetWindowTitle(window, title.str().c_str());
        lastTime = currentTime;
        numFrames = -1;
        frameTime = static_cast<float>(1000.0 / fps);
    }
    numFrames++;
}

App::~App() {
    delete graphicsEngine;
}


