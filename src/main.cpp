//
// Created by itari on 2026-02-24.
//

#include <iostream>

#include "src/config.h"
#include "src/VulkanEngine.h"

int main() {

    auto engine = VulkanEngine();
    engine.run();

    return 0;
}
