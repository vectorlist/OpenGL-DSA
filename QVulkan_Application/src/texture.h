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

	VkSampler sampler;
	VkImage image;
	VkImageLayout imageLayout;
	VkDeviceMemory memory;
	VkDescriptorImageInfo descriptor;
	uint32_t width;
	uint32_t height;
	uint32_t mipLevels;

	void buildTextureImage(const std::string &filename);


	//built in
	void createImage(
		uint32_t width,
		uint32_t height,
		VkFormat format,
		VkImageTiling tiling,
		VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkImage &image,
		VkDeviceMemory imageMemory);
};

