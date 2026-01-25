//
// Created by itari on 2026-01-24.
//

#ifndef HYDROSIM_ENGINE_H
#define HYDROSIM_ENGINE_H
#include "GLFW/glfw3.h"


class Engine {
public:
    Engine();
    ~Engine();
private:
    bool debugMode = true;

    int width{ 640 };
    int height { 480 };
    GLFWwindow* window;

    void build_glfw_window();
};


#endif //HYDROSIM_ENGINE_H