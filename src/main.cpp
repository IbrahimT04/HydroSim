#include "Engine.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

int main() {

    const auto* graphicsEngine = new Engine();

    delete graphicsEngine;

    return 0;
}