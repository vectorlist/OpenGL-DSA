#pragma once

#include <vulkan/vulkan.h>
#include <string>

class VulkanDevice;
class VulkanTexture
{
public:
	VulkanTexture(VulkanDevice* m_vulkanDevice);
	~VulkanTexture();

	VulkanDevice* m_vulkanDevice;
	VkDevice m_device;
	VkPhysicalDevice m_physicalDevice;
	void buildTexture(const std::string &filename);


	VkImage m_image;
	VkDeviceMemory m_memory;



};

