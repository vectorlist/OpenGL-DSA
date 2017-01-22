#pragma once

#include <vulkan/vulkan.h>
#include <assert.h>

class Buffer
{
public:
	VkBuffer buffer;
	VkDevice device;

	VkDeviceMemory memory;
	VkDescriptorBufferInfo descriptor;

	VkDeviceSize size = 0;
	VkDeviceSize alignment = 0;
	void* mapped = nullptr;

	VkBufferUsageFlags usageFlags;

	VkMemoryPropertyFlags memoryPropertyFlags;

	VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	void unmap();
	VkResult bind(VkDeviceSize offset = 0);
	void setupDescriptor(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	void copyTo(void* data, VkDeviceSize size);
	VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

	void destroy();

	//static 
	//void createBufferPool()
};

inline VkResult Buffer::map(VkDeviceSize size, VkDeviceSize offset)
{
	return vkMapMemory(device, memory, offset, size, 0, &mapped);
}

inline void Buffer::unmap()
{
	if (mapped)
	{
		vkUnmapMemory(device, memory);
		mapped = nullptr;
	}
}

inline VkResult Buffer::bind(VkDeviceSize offset)
{
	return vkBindBufferMemory(device, buffer, memory, offset);
}

inline void Buffer::setupDescriptor(VkDeviceSize size, VkDeviceSize offset)
{
	descriptor.offset = offset;
	descriptor.buffer = buffer;
	descriptor.range = size;
}

inline void Buffer::copyTo(void* data, VkDeviceSize size)
{
	assert(mapped);
	memcpy(mapped, data, size);
}

inline VkResult Buffer::flush(VkDeviceSize size, VkDeviceSize offset)
{
	VkMappedMemoryRange mappedRange = {};
	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.memory = memory;
	mappedRange.offset = offset;
	mappedRange.size = size;
	
	return vkFlushMappedMemoryRanges(device, 1, &mappedRange);
}

inline VkResult Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset)
{
	VkMappedMemoryRange mappedRange = {};
	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.memory = memory;
	mappedRange.offset = offset;
	mappedRange.size = size;
	return vkInvalidateMappedMemoryRanges(device, 1, &mappedRange);
}

inline void Buffer::destroy()
{
	if (buffer)
		vkDestroyBuffer(device, buffer, nullptr);
	if (memory)
		vkFreeMemory(device, memory, nullptr);
}