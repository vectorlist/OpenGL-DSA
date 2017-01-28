#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <vktools.h>
#include <vkinitializer.h>

struct QueueFamilyIndice
{
	uint32_t graphics;
	uint32_t compute;
	uint32_t transfer;
};

class VulkanDevice
{
public:
	VulkanDevice(VkInstance &instance, VkDevice &device, VkPhysicalDevice &physicalDevice);
	~VulkanDevice();

	VkInstance &m_instance;
	VkDevice &m_device;
	VkPhysicalDevice &m_physicalDevice;

	VkPhysicalDeviceProperties m_properties;
	VkPhysicalDeviceFeatures m_features;
	VkPhysicalDeviceMemoryProperties m_memoryProperties;
	std::vector<VkQueueFamilyProperties> m_queueFamilyProperties;
	std::vector<std::string> m_supportedExtensions;

	//instance queue for logical device
	VkQueue m_queue;

	
	QueueFamilyIndice m_queueFamilyIndices;
	VkCommandPool m_commandPool;


	void buildPhysicalDevice();
	void buildLogicalDevice(
		VkPhysicalDeviceFeatures enabledFeatures,
		bool useSwapcahin = true,
		VkQueueFlags requestType = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);

	bool extensionSupported(std::string extension);
	void getGraphicsQueue(VkQueue *queue);
	VkBool32 getSupportedDepthFormat(VkFormat *depthFormat);

	/*BUILT IN*/
	uint32_t getQueueFamilyIndex(VkQueueFlagBits queueFlags);
	VkCommandPool createCommandPool(
		uint32_t queueFamilyIndex,
		VkCommandPoolCreateFlags createFlags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	VkCommandBuffer createCommandBuffer(
		VkCommandBufferLevel level, 
		bool begin = false);

	void flushCommandBuffer(
		VkCommandBuffer commandBuffer,
		VkQueue queue, 
		bool free = true);

	uint32_t getMemoryType(
		uint32_t typeBits,
		VkMemoryPropertyFlags properties,
		VkBool32 *memTypeFound = nullptr);

	void createBuffer(
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer &buffer,
		VkDeviceMemory &bufferMemory,
		VkDeviceSize size
	);
	void copyBuffer(
		VkBuffer srcBuffer,
		VkBuffer dstBuffer,
		VkDeviceSize size
	);

	void destroyBuffer(VkBuffer buffer, VkDeviceMemory bufferMemory);
};

inline uint32_t VulkanDevice::getQueueFamilyIndex(VkQueueFlagBits queueFlags)
{
	// Dedicated queue for compute
	// Try to find a queue family index that supports compute but not graphics
	if (queueFlags & VK_QUEUE_COMPUTE_BIT)
	{
		for (uint32_t i = 0; i <
			static_cast<uint32_t>(m_queueFamilyProperties.size()); i++)
		{
			if ((m_queueFamilyProperties[i].queueFlags & queueFlags)
				&& ((m_queueFamilyProperties[i].queueFlags &
					VK_QUEUE_GRAPHICS_BIT) == 0))
			{
				return i;
				break;
			}
		}
	}

	// Dedicated queue for transfer
	// Try to find a queue family index that supports transfer but not graphics and compute
	if (queueFlags & VK_QUEUE_TRANSFER_BIT)
	{
		for (uint32_t i = 0; i < static_cast<uint32_t>(m_queueFamilyProperties.size()); i++)
		{
			if ((m_queueFamilyProperties[i].queueFlags & queueFlags) &&
				((m_queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) &&
				((m_queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
			{
				return i;
				break;
			}
		}
	}

	// For other queue types or if no separate compute queue is present, return the first one to support the requested flags
	for (uint32_t i = 0; i < static_cast<uint32_t>(m_queueFamilyProperties.size()); i++)
	{
		if (m_queueFamilyProperties[i].queueFlags & queueFlags)
		{
			return i;
			break;
		}
	}

	LOG_ASSERT("couldnt find a amtaching queue family index");
}

inline VkCommandPool VulkanDevice::createCommandPool(
	uint32_t queueFamilyIndex, 
	VkCommandPoolCreateFlags createFlags)
{
	VkCommandPoolCreateInfo cmdPoolInfo = {};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.queueFamilyIndex = queueFamilyIndex;
	cmdPoolInfo.flags = createFlags;
	VkCommandPool cmdPool;
	LOG_ERROR("failed to create command pool") <<
	vkCreateCommandPool(m_device, &cmdPoolInfo, nullptr, &cmdPool);
	return cmdPool;
}

inline VkCommandBuffer VulkanDevice::createCommandBuffer(VkCommandBufferLevel level, bool begin)
{
	VkCommandBufferAllocateInfo cmdBufAllocateInfo = 
		vkInitializer::commandBufferAllocateInfo(m_commandPool, level, 1);

	VkCommandBuffer cmdBuffer;
	LOG_ERROR("failed to create command buffer") <<
	vkAllocateCommandBuffers(m_device, &cmdBufAllocateInfo, &cmdBuffer);

	// If requested, also start recording for the new command buffer
	if (begin)
	{
		VkCommandBufferBeginInfo cmdBufInfo = vkInitializer::commandBufferBeginInfo();
		LOG_ERROR("failed to create begin command buffer") <<
		vkBeginCommandBuffer(cmdBuffer, &cmdBufInfo);
	}

	return cmdBuffer;
}

inline void VulkanDevice::flushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, bool free)
{
	if (commandBuffer == VK_NULL_HANDLE)
	{
		return;
	}

	LOG_ERROR("failed end command") <<
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = vkInitializer::submitInfo();
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	// Create fence to ensure that the command buffer has finished executing
	VkFenceCreateInfo fenceInfo = vkInitializer::fenceCreateInfo(VK_FLAGS_NONE);
	VkFence fence;
	LOG_ERROR("failed to create fence") <<
	vkCreateFence(m_device, &fenceInfo, nullptr, &fence);

	// Submit to the queue
	LOG_ERROR("failed to sumit to queue") <<
	vkQueueSubmit(queue, 1, &submitInfo, fence);
	// Wait for the fence to signal that command buffer has finished executing
	LOG_ERROR("failed to wait for fence") <<
	vkWaitForFences(m_device, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT);

	vkDestroyFence(m_device, fence, nullptr);

	if (free)
	{
		vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
	}
}

inline bool VulkanDevice::extensionSupported(std::string extension)
{
	return (std::find(m_supportedExtensions.begin(),
		m_supportedExtensions.end(), extension) != m_supportedExtensions.end());
}

inline uint32_t VulkanDevice::getMemoryType(
	uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32 *memTypeFound)
{
	for (uint32_t i = 0; i < m_memoryProperties.memoryTypeCount; ++i)
	{
		if ((typeBits & 1) == 1)
		{
			//LOG << "yeah" << ENDL;
			if ((m_memoryProperties.memoryTypes[i].propertyFlags & properties) ==
				properties)
			{
				if (memTypeFound)
				{
					*memTypeFound = true;
				}
				return i;
			}
		}
		typeBits >>= 1;
	}

	if (memTypeFound)
	{
		*memTypeFound = false;
		return 0;
	}
	else
	{
		qFatal("could not find a matching memorytype");
	}
}

