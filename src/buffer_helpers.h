//
// Created by itari on 2026-03-30.
//

#ifndef HYDROSIM_BUFFER_HELPERS_H
#define HYDROSIM_BUFFER_HELPERS_H

#include "config.h"

namespace vkBuffer {
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;

        static vk::VertexInputBindingDescription getBindingDescription() {
            return {0, sizeof(Vertex), vk::VertexInputRate::eVertex};
        }

        static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions() {
            return {
                vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos)),
                vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)),
                vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, texCoord))
            };
        }
    };

    struct UniformBufferObject {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };
}

#endif //HYDROSIM_BUFFER_HELPERS_H
