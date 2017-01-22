#pragma once

#include <vulkan/vulkan.h>

namespace vkInitializer
{
	inline VkRenderPassBeginInfo renderPassBeginInfo()
	{
		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = NULL;
		return renderPassBeginInfo;
	}

	inline VkViewport viewport(
		float width,
		float height,
		float minDepth,
		float maxDepth)
	{
		VkViewport viewport = {};
		viewport.width = width;
		viewport.height = height;
		viewport.minDepth = minDepth;
		viewport.maxDepth = maxDepth;
		return viewport;
	}

	inline VkRect2D rect2D(
		int32_t width,
		int32_t height,
		int32_t offsetX,
		int32_t offsetY)
	{
		VkRect2D rect2D = {};
		rect2D.extent.width = width;
		rect2D.extent.height = height;
		rect2D.offset.x = offsetX;
		rect2D.offset.y = offsetY;
		return rect2D;
	}

	inline VkBufferCreateInfo bufferCreateInfo()
	{
		VkBufferCreateInfo bufCreateInfo = {};
		bufCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		return bufCreateInfo;
	}

	inline VkBufferCreateInfo bufferCreateInfo(
		VkBufferUsageFlags usage,
		VkDeviceSize size)
	{
		VkBufferCreateInfo bufCreateInfo = {};
		bufCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufCreateInfo.pNext = NULL;
		bufCreateInfo.usage = usage;
		bufCreateInfo.size = size;
		bufCreateInfo.flags = 0;
		return bufCreateInfo;
	}


	inline VkMemoryAllocateInfo memoryAllocateInfo()
	{
		VkMemoryAllocateInfo memAllocInfo = {};
		memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memAllocInfo.pNext = NULL;
		memAllocInfo.allocationSize = 0;
		memAllocInfo.memoryTypeIndex = 0;
		return memAllocInfo;
	}

	inline VkMappedMemoryRange mappedMemoryRange()
	{
		VkMappedMemoryRange mappedMemoryRange = {};
		mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		return mappedMemoryRange;
	}

	inline VkCommandBufferAllocateInfo commandBufferAllocateInfo(
		VkCommandPool commandPool, VkCommandBufferLevel level, uint32_t bufferCount)
	{
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandPool = commandPool;
		commandBufferAllocateInfo.level = level;
		commandBufferAllocateInfo.commandBufferCount = bufferCount;
		return commandBufferAllocateInfo;
	}


	inline VkCommandPoolCreateInfo commandPoolCreateInfo()
	{
		VkCommandPoolCreateInfo cmdPoolCreateInfo = {};
		cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		return cmdPoolCreateInfo;
	}

	inline VkCommandBufferBeginInfo commandBufferBeginInfo()
	{
		VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
		cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufferBeginInfo.pNext = NULL;
		return cmdBufferBeginInfo;
	}

	inline VkFenceCreateInfo fenceCreateInfo(VkFenceCreateFlags flags)
	{
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = flags;
		return fenceCreateInfo;
	}

	inline VkEventCreateInfo eventCreateInfo()
	{
		VkEventCreateInfo eventCreateInfo = {};
		eventCreateInfo.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;
		return eventCreateInfo;
	}

	inline VkSubmitInfo submitInfo()
	{
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = NULL;
		return submitInfo;
	}
}