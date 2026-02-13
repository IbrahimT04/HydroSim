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
    numFrames++;

    double delta = currentTime - lastTime;
    if (delta >= 1.0) {
        double fps = numFrames / delta;
        double ms_per_frame = (delta * 1000.0) / numFrames;

        std::ostringstream title;
        title.setf(std::ios::fixed);
        title.precision(1);
        title << "Running " << windowName
              << " | FPS: " << fps
              << " | " << ms_per_frame << " ms";

        glfwSetWindowTitle(window, title.str().c_str());

        lastTime = currentTime;
        numFrames = 0;
        frameTime = static_cast<float>(ms_per_frame);
    }
}


App::~App() {
    delete graphicsEngine;
}


