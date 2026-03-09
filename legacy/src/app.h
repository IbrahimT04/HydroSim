//
// Created by itari on 2026-02-12.
//

#ifndef HYDROSIM_APP_H
#define HYDROSIM_APP_H
#define DEBUG_MODE 1

#include <GLFW/glfw3.h>
#include "Engine.h"
#include "scene.h"

class App {
public:
    App(const std::string &name, int width, int height);
    ~App();
    void run();

private:
    static constexpr bool debugMode = DEBUG_MODE;
    std::string windowName;
    Engine* graphicsEngine;
    GLFWwindow* window { nullptr };
    Scene* scene { nullptr };

    double lastTime, currentTime;
    int numFrames;
    float frameTime;

    void build_glfw_window(int width, int height);
    void calculate_frame_rate();
};

#endif //HYDROSIM_APP_H