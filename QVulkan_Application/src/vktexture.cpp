#include "vktexture.h"
#include <qimage.h>
#include <vktools.h>
#define STB_IMAGE_IMPLEMENTATION  
#include <std_image/stb_image.h>
#include <vkdevice.h>

VulkanTexture::VulkanTexture(VulkanDevice* m_vulkanDevice)
	: m_vulkanDevice(m_vulkanDevice)
{
	m_device = m_vulkanDevice->m_device;
	m_physicalDevice = m_vulkanDevice->m_physicalDevice;
}


VulkanTexture::~VulkanTexture()
{
	vkFreeMemory(m_device, m_memory, nullptr);
}

void VulkanTexture::buildTexture(const std::string &filename)
{
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels,
		STBI_rgb_alpha);

	VkDeviceSize imageSize = texWidth * texHeight * 4;
	if (pixels == nullptr)
		assert(0 && "failed");

	

}


