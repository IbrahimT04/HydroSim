//
// Created by itari on 2026-02-11.
//

#ifndef HYDROSIM_PIPELINE_H
#define HYDROSIM_PIPELINE_H

#include "config.h"
#include "shaders.h"
#include "render_structs.h"

namespace vkInit {
    struct GraphicsPipelineInBundle {
        vk::Device device;
        std::string vertexFilePath;
        std::string fragmentFilePath;
        vk::Extent2D swapchainExtent;
        vk::Format swapchainFormat;
    };

    struct GraphicsPipelineOutBundle {
        vk::PipelineLayout pipelineLayout;
        vk::RenderPass renderPass;
        vk::Pipeline pipeline;
    };

    inline vk::RenderPass make_renderpass(const vk::Device& device, const vk::Format& swapchainImageFormat, const bool debug) {
        vk::AttachmentDescription colorAttachment = {};
        colorAttachment.flags = vk::AttachmentDescriptionFlags();
        colorAttachment.format = swapchainImageFormat;
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

        vk::AttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::SubpassDescription subpass = {};
        subpass.flags = vk::SubpassDescriptionFlagBits();
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        vk::RenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.flags = vk::RenderPassCreateFlags();
        renderPassCreateInfo.attachmentCount = 1;
        renderPassCreateInfo.pAttachments = &colorAttachment;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;
        try {
            return device.createRenderPass(renderPassCreateInfo);
        }
        catch (vk::SystemError&) {
            if (debug) {
                std::cerr << "Failed create Renderpass!" << std::endl;
            }
            return nullptr;
        }
    }

    inline vk::PipelineLayout make_pipeline_layout(const vk::Device& device, const bool debug) {
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.flags = vk::PipelineLayoutCreateFlags();
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        vk::PushConstantRange pushConstantRange = {};
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(vkUtil::ObjectData);
        pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        try {
            return device.createPipelineLayout(pipelineLayoutInfo);
        }
        catch (const vk::SystemError&) {
            if (debug) {
                std::cerr << "Failed to create pipeline layout!" << std::endl;
            }
            return nullptr;
        }
    }

    inline GraphicsPipelineOutBundle make_graphics_pipeline(const GraphicsPipelineInBundle& specification, const bool debug) {

        vk::GraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.flags = vk::PipelineCreateFlags();

        std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

        // ------------------ Pipeline Steps ------------------
        // Step 1: Vertex Input
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.flags = vk::PipelineVertexInputStateCreateFlags();
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        pipelineInfo.pVertexInputState = &vertexInputInfo;

        // Step 2: Input Assembly
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyinfo = {};
        inputAssemblyinfo.flags = vk::PipelineInputAssemblyStateCreateFlags();
        inputAssemblyinfo.topology = vk::PrimitiveTopology::eTriangleList;
        pipelineInfo.pInputAssemblyState = &inputAssemblyinfo;

        // Step 3: Vertex Shader
        if (debug) {
            std::cout << "Creating Vertex shader module" << std::endl;
        }
        vk::ShaderModule vertexShader = vkUtil::createModule(specification.vertexFilePath, specification.device, debug);
        vk::PipelineShaderStageCreateInfo vertexShaderInfo = {};
        vertexShaderInfo.flags = vk::PipelineShaderStageCreateFlags();
        vertexShaderInfo.stage = vk::ShaderStageFlagBits::eVertex;
        vertexShaderInfo.module = vertexShader;
        vertexShaderInfo.pName = "main";
        shaderStages.emplace_back(vertexShaderInfo);

        // Step 4: Viewport and Scissor
        vk::Viewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(specification.swapchainExtent.width);
        viewport.height = static_cast<float>(specification.swapchainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vk::Rect2D scissorRect = {};
        scissorRect.offset.x = 0.0f;
        scissorRect.offset.y = 0.0f;
        scissorRect.extent = specification.swapchainExtent;
        vk::PipelineViewportStateCreateInfo viewportStateInfo = {};
        viewportStateInfo.flags = vk::PipelineViewportStateCreateFlags();
        viewportStateInfo.viewportCount = 1;
        viewportStateInfo.pViewports = &viewport;
        viewportStateInfo.scissorCount = 1;
        viewportStateInfo.pScissors = &scissorRect;
        pipelineInfo.pViewportState = &viewportStateInfo;

        // Step 5: Rasterizer
        vk::PipelineRasterizationStateCreateInfo rasterizerInfo = {};
        rasterizerInfo.flags = vk::PipelineRasterizationStateCreateFlags();
        rasterizerInfo.depthClampEnable = VK_FALSE;
        rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizerInfo.polygonMode = vk::PolygonMode::eFill;
        rasterizerInfo.lineWidth = 1.0f;
        rasterizerInfo.cullMode = vk::CullModeFlagBits::eBack;
        rasterizerInfo.frontFace = vk::FrontFace::eClockwise;
        rasterizerInfo.depthBiasEnable = VK_FALSE;
        pipelineInfo.pRasterizationState = &rasterizerInfo;

        // Step 6: Fragment Shader
        if (debug) {
            std::cout << "Creating Fragment shader module" << std::endl;
        }
        vk::ShaderModule fragmentShader = vkUtil::createModule(specification.fragmentFilePath, specification.device, debug);
        vk::PipelineShaderStageCreateInfo fragmentShaderInfo = {};
        fragmentShaderInfo.flags = vk::PipelineShaderStageCreateFlags();
        fragmentShaderInfo.stage = vk::ShaderStageFlagBits::eFragment;
        fragmentShaderInfo.module = fragmentShader;
        fragmentShaderInfo.pName = "main";
        shaderStages.emplace_back(fragmentShaderInfo);

        pipelineInfo.setStages(shaderStages);

        // Step 7: Multisampling
        vk::PipelineMultisampleStateCreateInfo multisampleInfo = {};
        multisampleInfo.flags = vk::PipelineMultisampleStateCreateFlags();
        multisampleInfo.sampleShadingEnable = VK_FALSE;
        multisampleInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
        pipelineInfo.pMultisampleState = &multisampleInfo;

        // Step 8: Color Blend
        vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR |
                                              vk::ColorComponentFlagBits::eG |
                                              vk::ColorComponentFlagBits::eB |
                                              vk::ColorComponentFlagBits::eA;
        colorBlendAttachment.blendEnable = VK_FALSE;

        vk::PipelineColorBlendStateCreateInfo colorBlendState = {};
        colorBlendState.flags = vk::PipelineColorBlendStateCreateFlags();
        colorBlendState.logicOpEnable = VK_FALSE;
        colorBlendState.logicOp = vk::LogicOp::eCopy;
        colorBlendState.attachmentCount = 1;
        colorBlendState.pAttachments = &colorBlendAttachment;
        colorBlendState.blendConstants[0] = 0.0f;
        colorBlendState.blendConstants[1] = 0.0f;
        colorBlendState.blendConstants[2] = 0.0f;
        colorBlendState.blendConstants[3] = 0.0f;
        pipelineInfo.pColorBlendState = &colorBlendState;

        // --------------- Pipeline Layout Build ---------------
        // Create Pipeline Layout:
        if (debug) {
            std::cout << "Creating Pipeline layout" << std::endl;
        }
        vk::PipelineLayout layout = make_pipeline_layout(specification.device, debug);
        pipelineInfo.layout = layout;

        // ----------------- RenderPass Build -----------------
        if (debug) {
            std::cout << "Create Renderpass" << std::endl;
        }
        vk::RenderPass renderpass = make_renderpass(specification.device, specification.swapchainFormat, debug);
        pipelineInfo.renderPass = renderpass;

        // Extra Stuff
        pipelineInfo.basePipelineHandle = nullptr;

        // ------------- Finally make the pipeline -------------
        if (debug) {
            std::cout << "Creating the Graphics Pipeline" << std::endl;
        }
        vk::Pipeline graphicsPipeline = {};
        try {
            graphicsPipeline = (specification.device.createGraphicsPipeline(nullptr, pipelineInfo)).value;
        }
        catch (const vk::SystemError& e) {
            if (debug) {
                std::cout << "Failed to create Graphics Pipeline!" << std::endl;
            }
        }

        GraphicsPipelineOutBundle result = {};
        result.pipelineLayout = layout;
        result.renderPass = renderpass;
        result.pipeline = graphicsPipeline;

        specification.device.destroyShaderModule(vertexShader);
        specification.device.destroyShaderModule(fragmentShader);

        return result;
    }
}

#endif //HYDROSIM_PIPELINE_H