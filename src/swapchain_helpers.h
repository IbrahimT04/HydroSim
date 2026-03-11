//
// Created by itari on 2026-03-09.
//

#ifndef HYDROSIM_SWAPCHAIN_HELPERS_H
#define HYDROSIM_SWAPCHAIN_HELPERS_H

#include "config.h"

#include <limits>
#include <algorithm>

namespace vkSwapchain {
    inline vk::SurfaceFormatKHR choose_swapchain_surface_format(
        const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
        const auto formatIt = std::ranges::find_if(
            availableFormats,
            [](const auto &format) {
                return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace ==
                       vk::ColorSpaceKHR::eSrgbNonlinear;
            });
        return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
    }

    inline vk::PresentModeKHR choose_swapchain_present_mode(
        std::vector<vk::PresentModeKHR> const &availablePresentModes) {

        assert(std::ranges::any_of(availablePresentModes, [](auto presentMode) {
            return presentMode == vk::PresentModeKHR::eFifo;
        }));

        return std::ranges::any_of(availablePresentModes,
                                   [](const vk::PresentModeKHR value) {
                                       return vk::PresentModeKHR::eMailbox == value;
                                   }) ?
                   vk::PresentModeKHR::eMailbox :
                   vk::PresentModeKHR::eFifo;
    }


    inline vk::Extent2D choose_swapchain_extent(vk::SurfaceCapabilitiesKHR const &capabilities, GLFWwindow* window)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        return {
            std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
        };
    }
    inline uint32_t choose_swapchain_min_image_count(vk::SurfaceCapabilitiesKHR const &surfaceCapabilities)
    {
        auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
        if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount))
        {
            minImageCount = surfaceCapabilities.maxImageCount;
        }
        return minImageCount;
    }
}

#endif //HYDROSIM_SWAPCHAIN_HELPERS_H
