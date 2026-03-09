//
// Created by itari on 2026-02-16.
//

#ifndef HYDROSIM_MESH_H
#define HYDROSIM_MESH_H

#include "config.h"

namespace vkMesh {

    enum class BindingType { PositionColor };

    inline vk::VertexInputBindingDescription getPosColBindingDesc() {

        vk::VertexInputBindingDescription bindingDescription;
        bindingDescription.binding = 0;
        bindingDescription.stride = 6 * sizeof(float);
        bindingDescription.inputRate = vk::VertexInputRate::eVertex;

        return bindingDescription;
    }

    inline std::array<vk::VertexInputAttributeDescription, 2> getPosColorAttributeDesc() {
        std::array<vk::VertexInputAttributeDescription, 2> attributes;
        attributes[0].binding = 0;
        attributes[0].location = 0;
        attributes[0].format = vk::Format::eR32G32B32Sfloat;
        attributes[0].offset = 0;

        attributes[1].binding = 0;
        attributes[1].location = 1;
        attributes[1].format = vk::Format::eR32G32B32Sfloat;
        attributes[1].offset = 3 * sizeof(float);

        return attributes;
    }

    // Main attribute descritption function
    // inline vk::VertexInputAttributeDescription getBindingDesc(const BindingType bindingType) {
    //     if (bindingType == BindingType::PositionColor) {
    //         return getPosColorAttributeDesc();
    //     }
    //     return vk::VertexInputAttributeDescription{};
    // }

    // Main binding function
    inline vk::VertexInputBindingDescription getBindingDesc(const BindingType bindingType) {
        if (bindingType == BindingType::PositionColor) {
            return getPosColBindingDesc();
        }
        return vk::VertexInputBindingDescription{};
    }

}

#endif //HYDROSIM_MESH_H