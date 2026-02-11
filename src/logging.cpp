//
// Created by itari on 2026-01-26.
//
#include "logging.h"
#include <iostream>

#include "config.h"

namespace vkInit {
    VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
        vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        vk::DebugUtilsMessageTypeFlagsEXT messageType,
        const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData
    ) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << '\n';
        return VK_FALSE; // apps should return false :contentReference[oaicite:2]{index=2}
    }

    vk::DebugUtilsMessengerEXT make_debug_messeger(const vk::Instance &instance,
                                                   const vk::detail::DispatchLoaderDynamic &dldi) {
        constexpr auto createInfo = vk::DebugUtilsMessengerCreateInfoEXT{}
                .setMessageSeverity(
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
                .setMessageType(
                    vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                    vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                    vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
                .setPfnUserCallback(
                    static_cast<vk::PFN_DebugUtilsMessengerCallbackEXT>(debugCallback))

                .setPUserData(nullptr);

        return instance.createDebugUtilsMessengerEXT(createInfo, nullptr, dldi);
    }

    void log_device_properties(const vk::PhysicalDevice &device) {
        vk::PhysicalDeviceProperties properties = device.getProperties();

        std::cout << "Device Name: " << properties.deviceName << "\n";
        std::cout << "Device Type: ";
        switch (properties.deviceType) {
            case (vk::PhysicalDeviceType::eCpu):
                std::cout << "CPU\n";
                break;
            case (vk::PhysicalDeviceType::eDiscreteGpu):
                std::cout << "Discrete GPU\n";
                break;
            case (vk::PhysicalDeviceType::eIntegratedGpu):
                std::cout << "Integrated GPU\n";
                break;
            case (vk::PhysicalDeviceType::eVirtualGpu):
                std::cout << "Virtual GPU\n";
                break;
            case (vk::PhysicalDeviceType::eOther):
                std::cout << "Other GPU\n";
                break;
            default:
                std::cout << "UNKNOWN\n";
        }
    }

    std::vector<std::string> log_transform_bits(const vk::SurfaceTransformFlagsKHR bits) {
        std::vector<std::string> result;

        if (bits & vk::SurfaceTransformFlagBitsKHR::eIdentity) {
            result.emplace_back("identity");
        }
        if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate90) {
            result.emplace_back("90 degree rotation");
        }
        if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate180) {
            result.emplace_back("180 degree rotation");
        }
        if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate270) {
            result.emplace_back("270 degree rotation");
        }
        if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirror) {
            result.emplace_back("horizontal mirror");
        }
        if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate90) {
            result.emplace_back("horizontal mirror, then 90 degree rotation");
        }
        if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate180) {
            result.emplace_back("horizontal mirror, then 180 degree rotation");
        }
        if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate270) {
            result.emplace_back("horizontal mirror, then 270 degree rotation");
        }
        if (bits & vk::SurfaceTransformFlagBitsKHR::eInherit) {
            result.emplace_back("inherited");
        }
        return result;
    }

    std::vector<std::string> log_alpha_composite_bits(const vk::CompositeAlphaFlagsKHR bits) {
        std::vector<std::string> result;
        if (bits & vk::CompositeAlphaFlagBitsKHR::eOpaque) {
            result.emplace_back("Opaque, (Alpha ignored)");
        }
        if (bits & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied) {
            result.emplace_back("Premultiplied, (Alpha expected to already be multiplied)");
        }
        if (bits & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied) {
            result.emplace_back("Postmultiplied, (Alpha will be applied during composition)");
        }
        if (bits & vk::CompositeAlphaFlagBitsKHR::eInherit) {
            result.emplace_back("inherited");
        }

        return result;
    }

    std::vector<std::string> log_image_usage_bits(const vk::ImageUsageFlags bits) {
        std::vector<std::string> result;

        if (bits & vk::ImageUsageFlagBits::eTransferSrc) {
            result.emplace_back("transfer src: source for copy/transfer.");
        }
        if (bits & vk::ImageUsageFlagBits::eTransferDst) {
            result.emplace_back("transfer dst: destination for copy/transfer.");
        }
        if (bits & vk::ImageUsageFlagBits::eSampled) {
            result.emplace_back("sampled: shader-sampled image view.");
        }
        if (bits & vk::ImageUsageFlagBits::eStorage) {
            result.emplace_back("storage: shader storage image view.");
        }
        if (bits & vk::ImageUsageFlagBits::eColorAttachment) {
            result.emplace_back("color attachment: framebuffer color/resolve attachment.");
        }
        if (bits & vk::ImageUsageFlagBits::eDepthStencilAttachment) {
            result.emplace_back("depth/stencil attachment: framebuffer depth/stencil attachment.");
        }
        if (bits & vk::ImageUsageFlagBits::eTransientAttachment) {
            result.emplace_back("transient attachment: transient framebuffer attachment (may be lazily allocated).");
        }
        if (bits & vk::ImageUsageFlagBits::eInputAttachment) {
            result.emplace_back("input attachment: framebuffer input attachment (shader read).");
        }
        if (bits & vk::ImageUsageFlagBits::eFragmentDensityMapEXT) {
            result.emplace_back("fragment density map: fragment density map image view.");
        }
        if (bits & vk::ImageUsageFlagBits::eFragmentShadingRateAttachmentKHR) {
            result.emplace_back("shading rate attachment: fragment shading rate image view.");
        }

        return result;
    }
} // namespace vkInit
