#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

# include "Engine.h"

int main() {

    const auto* graphicsEngine = new Engine();

    delete graphicsEngine;

    return 0;
}