#include "texture.h"
#include <std_image/stb_image.h>
#include <vklog.h>

#include <vkdevice.h>

Texture::Texture(VulkanDevice* vulkandevice)
	: vulkanDevice(vulkandevice)
{
	m_device = vulkanDevice->m_device;
}


Texture::~Texture()
{
}


void Texture::buildTextureImage(const std::string &filename)
{
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	VkDeviceSize imageSize = texWidth * texHeight * 4;
	if (pixels == nullptr)
		LOG_WARN("failed to load image");

	VkImage staginImage;
	VkDeviceMemory stagingMemory;


}


void Texture::createImage(
	uint32_t width,
	uint32_t height,
	VkFormat format,
	VkImageTiling tiling,
	VkImageUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkImage &image,
	VkDeviceMemory imageMemory)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_1D;
	imageInfo.extent = { width, height,1 };
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = tiling;
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

	LOG_ERROR("failed to create image") <<
		vkCreateImage(m_device, &imageInfo, nullptr, &image);

	VkMemoryRequirements memReqs{};
	vkGetImageMemoryRequirements(m_device, image, &memReqs);

	VkMemoryAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize - memReqs.size;
	allocateInfo.memoryTypeIndex = vulkanDevice->getMemoryType(memReqs.memoryTypeBits, properties);

	LOG_ERROR("failed to allocate image memory") <<
	vkAllocateMemory(m_device, &allocateInfo, nullptr, &imageMemory);

	vkBindImageMemory(m_device, image, imageMemory, 0);
}