
#include "Rendering/Context.hpp"
// #include "Wrapper/Base.hpp"
#include "Wrapper/Component.hpp"
#include "Wrapper/Device.hpp"
// #include "Wrapper/FrameBuffer.hpp"
// #include "Wrapper/Image.hpp"
#include <cassert>
#include <set>
#include <sys/types.h>
#include <vulkan/vulkan_structs.hpp>

namespace MCRT {
class Image;
class Framebuffer;
class SwapChain : public Component<vk::SwapchainKHR, SwapChain> {
public:
    SwapChain();
    ~SwapChain();

    struct SwapChain_Info {
        vk::Extent2D image_extent;
        uint32_t image_count;
        vk::SurfaceFormatKHR format;
    };
    struct SurfaceInfo {
        vk::SurfaceFormatKHR format;
        vk::Extent2D extent;
        uint32_t count;
        vk::SurfaceTransformFlagBitsKHR transform;
    } surfaceInfo;

    [[nodiscard("Missing Swapchain_Image")]] auto& Get_Swapchain_ImagesHandle()
    {
        assert(m_handle);
        if (swapchain_Image_handles.empty()) {
            swapchain_Image_handles = Context::Get_Singleton()
                                          ->get_device()
                                          ->get_handle()
                                          .getSwapchainImagesKHR(m_handle);
        }
        return swapchain_Image_handles;
    }
    auto& Get_Images()
    {
        assert(!images.empty());
        return images;
    }
    [[nodiscard("missing extent2D")]] auto Get_Extent2D()
    {
        return surfaceInfo.extent;
    }

    [[nodiscard("Missing format")]] auto Get_Format()
    {
        return surfaceInfo.format.format;
    }
    // [[nodiscard("Missing Images")]] auto Get_Swapchain_Images()
    // {
    //     return images;
    // }
    //    [[nodiscard("Missing FramgeBUffer")]] auto Get_FrameBuffers()
    //    {
    //        return framebuffers;
    //    }
    [[nodiscard("Missing  Swapchian_Images_Size")]] auto Get_Swapchain_Image_size()
    {
        return Get_Swapchain_ImagesHandle().size();
    }

private:
    static vk::SwapchainKHR old_swapchian;
    vk::SurfaceFormatKHR Query_surface_Format();
    void setImages();

    std::vector<std::shared_ptr<Image>> images;
    //    std::vector<std::shared_ptr<Framebuffer>> framebuffers;
    vk::Extent2D Query_surface_Extent(const vk::SurfaceCapabilitiesKHR& capability, int windowWidth, int windowHeight);
    std::vector<vk::Image> swapchain_Image_handles;
    void Query_info();
};

} // namespace MoCheng3D