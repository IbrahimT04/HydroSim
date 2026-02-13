//
// Created by itari on 2026-02-05.
//

#ifndef HYDROSIM_SWAPCHAIN_H
#define HYDROSIM_SWAPCHAIN_H

#include "config.h"
#include "logging.h"
#include "queues.h"
#include "frame.h"

namespace vkInit {
    struct SwapChainSupportDetails {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    struct SwapChainBundle {
        vk::SwapchainKHR swapchain;
        vk::Extent2D extent;
        vk::Format format;
        std::vector<vkUtil::SwapchainFrame> frames;
    };

    inline SwapChainSupportDetails query_swapchain_support(const vk::PhysicalDevice& device,
                                                           const vk::SurfaceKHR& surface, const bool debug) {
        SwapChainSupportDetails support;
        support.capabilities = device.getSurfaceCapabilitiesKHR(surface);

        if (debug) {
            std::cout << "Swapchain can support the following capabilities: \n";

            std::cout << "\t Minimum image count: " << support.capabilities.minImageCount << "\n";
            std::cout << "\t Maximum image count: " << support.capabilities.maxImageCount << "\n";

            std::cout << "\t Current image extent: \n";
            std::cout << "\t \t Width: " << support.capabilities.currentExtent.width << "\n";
            std::cout << "\t \t Height: " << support.capabilities.currentExtent.height << "\n";

            std::cout << "\t Minimum image extent: \n";
            std::cout << "\t \t Width: " << support.capabilities.minImageExtent.width << "\n";
            std::cout << "\t \t Height: " << support.capabilities.minImageExtent.height << "\n";

            std::cout << "\t Maximum image extent: \n";
            std::cout << "\t \t Width: " << support.capabilities.maxImageExtent.width << "\n";
            std::cout << "\t \t Height: " << support.capabilities.maxImageExtent.height << "\n";

            std::cout << "\t Max image array layers: " << support.capabilities.maxImageArrayLayers << "\n";

            std::cout << "\t Supported Transforms:\n";
            for (const std::vector<std::string> stringList =
                         log_transform_bits(support.capabilities.supportedTransforms);
                 const std::string& s: stringList) {
                std::cout << "\t \t" << s << "\n";
            }

            std::cout << "\t Current Transforms:\n";
            for (const std::vector<std::string> stringList =
                         log_transform_bits(support.capabilities.currentTransform);
                 const std::string& s: stringList) {
                std::cout << "\t \t" << s << "\n";
            }

            std::cout << "\t Supported Alpha Options:\n";
            for (const std::vector<std::string> stringList =
                         log_alpha_composite_bits(support.capabilities.supportedCompositeAlpha);
                 const std::string& s: stringList) {
                std::cout << "\t \t" << s << "\n";
            }

            std::cout << "\t Supported Image Usage:\n";
            for (const std::vector<std::string> stringList =
                         log_image_usage_bits(support.capabilities.supportedUsageFlags);
                 const std::string& s: stringList) {
                std::cout << "\t \t" << s << "\n";
            }
        }

        support.formats = device.getSurfaceFormatsKHR(surface);

        if (debug) {
            for (const auto supportedFormat: support.formats) {
                std::cout << "Supported pixel format: " << vk::to_string(supportedFormat.format) << '\n';
                std::cout << "Supported color space: " << vk::to_string(supportedFormat.colorSpace) << "\n\n";
            }
        }

        support.presentModes = device.getSurfacePresentModesKHR(surface);

        if (debug) {
            std::cout << "Supported present modes:\n";
            for (const auto presentMode: support.presentModes) {
                std::cout << '\t' << vk::to_string(presentMode) << '\n';
            }
        }

        return support;
    }

    inline vk::SurfaceFormatKHR choose_swapchain_surface_format(const std::vector<vk::SurfaceFormatKHR> &formats) {
        for (const auto format: formats) {
            if (format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                return format;
            }
        }
        return formats[0];
    }

    inline vk::PresentModeKHR choose_swapchain_present_mode(const std::vector<vk::PresentModeKHR> &presentModes) {
        for (const auto mode: presentModes) {
            if (mode == vk::PresentModeKHR::eMailbox) {
                return mode;
            }
        }
        return vk::PresentModeKHR::eFifo;
    }

    inline vk::Extent2D choose_swapchain_extent(const uint32_t width, const uint32_t height,
                                                const vk::SurfaceCapabilitiesKHR &capabilities) {
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        }

        vk::Extent2D extent = {width, height};
        extent.width = std::min(capabilities.maxImageExtent.width, std::max(capabilities.minImageExtent.width, width));
        extent.height = std::min(capabilities.maxImageExtent.height,
                                 std::max(capabilities.minImageExtent.height, height));
        return extent;
    }

    inline SwapChainBundle create_swapchain(const vk::Device& logicalDevice, const vk::PhysicalDevice& physicalDevice,
                                            vk::SurfaceKHR& surface, const int width, const int height,
                                            const bool debug) {
        const SwapChainSupportDetails support = query_swapchain_support(physicalDevice, surface, debug);

        const vk::SurfaceFormatKHR format = choose_swapchain_surface_format(support.formats);
        const vk::PresentModeKHR presentMode = choose_swapchain_present_mode(support.presentModes);
        const vk::Extent2D extent = choose_swapchain_extent(width, height, support.capabilities);

        const uint32_t imageCount = std::min(support.capabilities.minImageCount + 1, support.capabilities.maxImageCount);

        auto createInfo = vk::SwapchainCreateInfoKHR(
            vk::SwapchainCreateFlagsKHR(),
            surface, imageCount,
            format.format, format.colorSpace,
            extent, 1,
            vk::ImageUsageFlagBits::eColorAttachment
        );

        QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface, debug);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily.value() != indices.presentFamily.value()) {
            createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        }

        createInfo.presentMode = presentMode;
        createInfo.preTransform = support.capabilities.currentTransform;
        createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        createInfo.clipped = VK_TRUE;

        createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

        SwapChainBundle bundle{};
        try {
            bundle.swapchain = logicalDevice.createSwapchainKHR(createInfo);
        }
        catch (const vk::SystemError&) {
            throw std::runtime_error("Failed to create swapchain");
        }

        std::vector<vk::Image> images = logicalDevice.getSwapchainImagesKHR(bundle.swapchain);
        bundle.frames.resize(images.size());
        for (size_t i = 0; i < images.size(); i++) {

            vk::ImageViewCreateInfo createImageInfo = {};
            createImageInfo.image = images[i];
            createImageInfo.viewType = vk::ImageViewType::e2D;
            createImageInfo.components.r = vk::ComponentSwizzle::eIdentity;
            createImageInfo.components.g = vk::ComponentSwizzle::eIdentity;
            createImageInfo.components.b = vk::ComponentSwizzle::eIdentity;
            createImageInfo.components.a = vk::ComponentSwizzle::eIdentity;
            createImageInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            createImageInfo.subresourceRange.baseMipLevel = 0;
            createImageInfo.subresourceRange.levelCount = 1;
            createImageInfo.subresourceRange.baseArrayLayer = 0;
            createImageInfo.subresourceRange.layerCount = 1;
            createImageInfo.format = format.format;

            bundle.frames[i].image = images[i];
            bundle.frames[i].imageView = logicalDevice.createImageView(createImageInfo);
        }
        bundle.format = format.format;
        bundle.extent = extent;

        return bundle;
    }
}

#endif //HYDROSIM_SWAPCHAIN_H
