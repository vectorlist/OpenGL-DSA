#pragma once

#include <vulkan/vulkan.h>
#include <vector>

typedef struct SwapCahinBuffer_T
{
	VkImage image;
	VkImageView view;
}SwapChainBuffer;

class VulkanSwapchain
{
public:
	VulkanSwapchain(VkInstance &instance, VkDevice &device, 
		VkPhysicalDevice &physicaldevice, VkSurfaceKHR &surface);
	~VulkanSwapchain();

	VkInstance &m_instance;
	VkPhysicalDevice &m_physicalDevice;
	VkDevice &m_device;
	VkSurfaceKHR &m_surface;

	VkFormat m_colorFormat;
	VkColorSpaceKHR m_colorSpace;

	VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;

	uint32_t m_imageCount;

	std::vector<VkImage> m_images;
	std::vector<SwapChainBuffer> m_buffers;

	uint32_t m_queueNodeIndex = UINT32_MAX;

	void init();
	void buildSwapchain(uint32_t *width, uint32_t *height, bool vsync = false);

	VkResult acquireNextimage(VkSemaphore presentCompleteSemaphores, uint32_t *imageIndex);
	VkResult queuePresent(VkQueue queue, uint32_t imageIndex,
		VkSemaphore waitSemaphore = VK_NULL_HANDLE);

	void clean();
};

