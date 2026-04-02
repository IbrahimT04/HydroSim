//
// Created by itari on 2026-03-30.
//

#ifndef HYDROSIM_BUFFER_HELPERS_H
#define HYDROSIM_BUFFER_HELPERS_H

#include "config.h"

namespace vkBuffer {
    struct Vertex
    {
        glm::vec2 pos;
        glm::vec3 color;

        static vk::VertexInputBindingDescription getBindingDescription() {
            return { 0, sizeof(Vertex), vk::VertexInputRate::eVertex };
        }
        static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions() {
            return {
                vk::VertexInputAttributeDescription( 0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos) ),
                vk::VertexInputAttributeDescription( 1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color) )
            };
        }
    };
    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };
}

#endif //HYDROSIM_BUFFER_HELPERS_H