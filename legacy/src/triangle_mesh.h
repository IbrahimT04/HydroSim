//
// Created by itari on 2026-02-21.
//

#ifndef HYDROSIM_TRIANGLE_MESH_H
#define HYDROSIM_TRIANGLE_MESH_H

#include "config.h"
#include "memory.h"

namespace vkMesh {
    class TriangleMesh {
        public:
            TriangleMesh(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice);
            ~TriangleMesh();
            vkUtil::Buffer vertexBuffer;
        private:
            vk::Device logicalDevice;
    };
}

#endif //HYDROSIM_TRIANGLE_MESH_H