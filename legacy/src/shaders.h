//
// Created by itari on 2026-02-10.
//

#ifndef HYDROSIM_SHADERS_H
#define HYDROSIM_SHADERS_H

#include "config.h"

namespace vkUtil {

    inline std::vector<char> readFile(const std::string& filePath, const bool debug) {

        std::ifstream file(filePath, std::ios::ate | std::ios::binary);
        if (debug && !file.is_open()) {
            std::cerr << "Can't open file: \"" << filePath << "\"" << std::endl;
        }

        const size_t fileSize{ static_cast<size_t>(file.tellg()) };

        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();
        return buffer;
    }

    inline vk::ShaderModule createModule(const std::string& filename, const vk::Device& device, const bool debug) {

        std::vector<char> code = readFile(filename, debug);

        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.flags = vk::ShaderModuleCreateFlags();
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        try {
            return device.createShaderModule(createInfo);
        }
        catch (const vk::SystemError&) {
            if (debug) {
                std::cout << "Failed to create shader module: " << std::endl;
            }
            return nullptr;
        }
    }
}

#endif //HYDROSIM_SHADERS_H