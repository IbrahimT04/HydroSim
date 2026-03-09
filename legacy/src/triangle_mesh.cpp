//
// Created by itari on 2026-02-21.
//

#include "triangle_mesh.h"

vkMesh::TriangleMesh::TriangleMesh(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice) {
    this->logicalDevice = logicalDevice;

    std::vector<float> vertices = {
        {
            0.0f, -0.05f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.05f, 0.05f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.05f, 0.05f, 0.0f, 0.0f, 1.0f, 0.0f,
        }};

    vkUtil::BufferInput inputChunk;
    inputChunk.device = logicalDevice;
    inputChunk.physicalDevice = physicalDevice;
    inputChunk.size = sizeof(float) * vertices.size();
    inputChunk.usage = vk::BufferUsageFlagBits::eVertexBuffer;

    vertexBuffer = vkUtil::createBuffer(inputChunk);

    void* mappedLocation = logicalDevice.mapMemory(vertexBuffer.memory, 0, inputChunk.size);
    memcpy(mappedLocation, vertices.data(), inputChunk.size);
    logicalDevice.unmapMemory(vertexBuffer.memory);
}

vkMesh::TriangleMesh::~TriangleMesh() {
    logicalDevice.destroyBuffer(vertexBuffer.buffer);
    logicalDevice.freeMemory(vertexBuffer.memory);
}


