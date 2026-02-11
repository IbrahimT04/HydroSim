#include "Engine.h"
#include "query_system.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

int main() {

    ioUtil::print_system_type();

    const auto* graphicsEngine = new Engine();

    delete graphicsEngine;

    return 0;
}