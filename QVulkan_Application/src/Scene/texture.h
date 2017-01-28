#pragma once

#include <vulkan/vulkan.h>
#include <string>

class VulkanDevice;
class Texture
{
public:
	Texture(VulkanDevice *vulkandevice);
	~Texture();

	VulkanDevice* vulkanDevice;
	VkDevice m_device;
	VkPhysicalDevice m_physicalDevice;

	VkSampler sampler;
	VkImage image;
	VkImageView view;
	VkImageLayout imageLayout;
	VkDeviceMemory memory;
	VkDescriptorImageInfo descriptor;
	uint32_t width;
	uint32_t height;
	uint32_t mipLevels;

	void loadTexture(const std::string &filename, VkFormat format, bool forceLinearTiling);
	//built in
	
	void setImageLayout(
		VkCommandBuffer cmdBuffer,
		VkImage image,
		VkImageAspectFlags aspectMask,
		VkImageLayout oldImageLayout,
		VkImageLayout newImageLayout,
		VkImageSubresourceRange subresourceRange);

	//VkFormat covertQImageToVKFormat(QImage::Format format);
};

