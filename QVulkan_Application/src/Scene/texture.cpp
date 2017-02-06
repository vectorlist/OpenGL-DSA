
#include "texture.h"
#include <vklog.h>
#include <vkdevice.h>
#include <qimage.h>

Texture::Texture(VulkanDevice* vulkandevice)
	: vulkanDevice(vulkandevice)
{
	m_device = vulkanDevice->m_device;
	m_physicalDevice = vulkanDevice->m_physicalDevice;
}


Texture::~Texture()
{
	vkDestroyImageView(m_device, view, nullptr);
	vkDestroyImage(m_device, image, nullptr);
	vkDestroySampler(m_device, sampler, nullptr);
	vkFreeMemory(m_device, memory, nullptr);
}



//#include <qopengltexture.h>
#include <mipmap.h>

void Texture::loadTexture(const std::string &filename, VkFormat format, bool forceLinearTiling)
{
	QImage file(filename.c_str());
	QImage img = file.convertToFormat(QImage::Format_RGBA8888);
	//QImage img = file.convertToFormat(QImage::Format_RGB888);
	
	/*auto* pixels = img.bits();*/
	LOG_SECTION("create mipmaps ..");
	Mipmap mip(file.width(), file.height(), file.constBits(),3);
	LOG << "max MipMaps : " << mip.pyramids.size() << ENDL;
	auto* buffer = mip.mapBuffer(0);
	auto* pixels = buffer->data();

	VkDeviceSize imageSize = buffer->width * buffer->height * 4;
	
	width = buffer->width;
	height = buffer->height;
	mipLevels = 1;

	VkFormatProperties formatProperties;

	vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &formatProperties);

	VkBool32 useStaging = true;
	if (forceLinearTiling)
		useStaging = !(formatProperties.linearTilingFeatures & 
			VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);

	VkMemoryAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.pNext = NULL;
	allocateInfo.allocationSize = 0;
	allocateInfo.memoryTypeIndex = 0;

	VkMemoryRequirements memReqs{};

	if (useStaging)
	{
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingMemory;

		//uint32_t zulsize = sizeof(pixels);

		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = imageSize;
		bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		LOG_ERROR("failed to create stagin buffer") <<
		vkCreateBuffer(m_device, &bufferCreateInfo, nullptr, &stagingBuffer);

		vkGetBufferMemoryRequirements(m_device, stagingBuffer, &memReqs);
		allocateInfo.allocationSize = memReqs.size;
		allocateInfo.memoryTypeIndex = vulkanDevice->getMemoryType(memReqs.memoryTypeBits, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

		vkAllocateMemory(m_device, &allocateInfo, nullptr, &stagingMemory);
		vkBindBufferMemory(m_device, stagingBuffer, stagingMemory, 0);

		uint8_t* data;
		//get available host memory address and  reservation memory size type of flag
		LOG_ERROR("failed to get host memory address") <<
		vkMapMemory(m_device, stagingMemory, 0, memReqs.size, 0, (void**)&data);
		//memcpy(data, pixels, (size_t)imageSize);
		memcpy(data, pixels, (size_t)imageSize);
		vkUnmapMemory(m_device, stagingMemory);

		std::vector<VkBufferImageCopy> bufferCopyRegions;
		uint32_t offset = 0;

		for (uint32_t i = 0; i < mipLevels; ++i)
		{
			VkBufferImageCopy bufferCopyRegion = {};
			bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			bufferCopyRegion.imageSubresource.mipLevel = i;
			bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
			bufferCopyRegion.imageSubresource.layerCount = 1;
			bufferCopyRegion.imageExtent.width = width;
			bufferCopyRegion.imageExtent.height = height;
			bufferCopyRegion.imageExtent.depth = 1;
			bufferCopyRegion.bufferOffset = offset;

			bufferCopyRegions.push_back(bufferCopyRegion);
			offset += (uint32_t)imageSize;
		}

		//create optimal tiled target image
		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.pNext = NULL;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = format;
		imageCreateInfo.mipLevels = mipLevels;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.extent = { width, height, 1 };
		imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

		LOG_ERROR("failed ro create image") <<
		vkCreateImage(m_device, &imageCreateInfo, nullptr, &image);

		vkGetImageMemoryRequirements(m_device, image, &memReqs);

		allocateInfo.allocationSize = memReqs.size;
		allocateInfo.memoryTypeIndex = vulkanDevice->getMemoryType(memReqs.memoryTypeBits,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		LOG_ERROR("failed to create image memory") <<
		vkAllocateMemory(m_device, &allocateInfo, nullptr, &memory);
		vkBindImageMemory(m_device, image, memory, 0);

		VkCommandBuffer cmdBuffer = vulkanDevice->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

		VkImageSubresourceRange subresourceRange{};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = mipLevels;
		subresourceRange.layerCount = 1;

		setImageLayout(
			cmdBuffer, image,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			subresourceRange);

		vkCmdCopyBufferToImage(
			cmdBuffer,
			stagingBuffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			(uint32_t)bufferCopyRegions.size(), bufferCopyRegions.data());
		
		imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		setImageLayout(
			cmdBuffer, image,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			imageLayout,
			subresourceRange);

		vulkanDevice->flushCommandBuffer(cmdBuffer, vulkanDevice->m_queue, true);

		vkFreeMemory(m_device, stagingMemory, nullptr);
		vkDestroyBuffer(m_device, stagingBuffer, nullptr);
	}
	else
	{
		LOG_WARN("implented");
	}

	//create sampler
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.pNext = NULL;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
	samplerInfo.maxLod = 0.0f;
	
	samplerInfo.maxLod = (useStaging) ? mipLevels : 0.0f;

	if (vulkanDevice->m_features.samplerAnisotropy)
	{
		samplerInfo.maxAnisotropy = vulkanDevice->m_properties.limits.maxSamplerAnisotropy;
		samplerInfo.anisotropyEnable = VK_TRUE;
	}
	else
	{
		samplerInfo.maxAnisotropy = 1.0f;
		samplerInfo.anisotropyEnable = VK_FALSE;
	}
	
	samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	LOG_ERROR("failed to create sampler") <<
	vkCreateSampler(m_device, &samplerInfo, nullptr, &sampler);

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.pNext = NULL;
	viewInfo.image = NULL;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.components = {
		VK_COMPONENT_SWIZZLE_R,VK_COMPONENT_SWIZZLE_G,
		VK_COMPONENT_SWIZZLE_B,VK_COMPONENT_SWIZZLE_A
	};
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	viewInfo.subresourceRange.levelCount = (useStaging) ? mipLevels : 1;
	viewInfo.image = image;

	LOG_ERROR("failed to create image view") <<
	vkCreateImageView(m_device, &viewInfo, nullptr, &view);

	descriptor.imageLayout = imageLayout;
	descriptor.imageView = view;
	descriptor.sampler = sampler;
}

void Texture::setImageLayout(
	VkCommandBuffer cmdBuffer,
	VkImage image,
	VkImageAspectFlags aspectMask,
	VkImageLayout oldImageLayout,
	VkImageLayout newImageLayout,
	VkImageSubresourceRange subresourceRange)
{
	VkImageMemoryBarrier imageMemoryBarrier{};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.pNext = NULL;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange = subresourceRange;

	switch (oldImageLayout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED :
		imageMemoryBarrier.srcAccessMask = 0;
		break;
	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	}

	switch (newImageLayout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	}

	VkPipelineStageFlags srcStageFlag = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags dstStageFlag = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	vkCmdPipelineBarrier(
		cmdBuffer,
		srcStageFlag,
		dstStageFlag,
		VK_FLAGS_NONE,
		0, nullptr,
		0, nullptr,
		1, &imageMemoryBarrier);

}


//VkFormat Texture::covertQImageToVKFormat(QImage::Format format)
//{
//	switch (format)
//	{
//	case QImage::Format::Format_RGB888:
//		return VK_FORMAT_R8G8B8_UNORM;
//	case QImage::Format::Format_RGBA8888:
//		return VK_FORMAT_R8G8B8A8_UNORM;
//	}
//	return VK_FORMAT_UNDEFINED;
//}