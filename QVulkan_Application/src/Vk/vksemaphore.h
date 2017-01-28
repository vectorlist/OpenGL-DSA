#pragma once

#include <vktools.h>

class VulkanSemaphore
{
public:
	VulkanSemaphore(VkDevice &device) : m_device(device) {}
	~VulkanSemaphore();
	VkDevice &m_device;
	VkSemaphore presentComplete;
	VkSemaphore renderComplete;
	VkSemaphore textOverlayComplete;

	void buildSemaphores();
};

inline void VulkanSemaphore::buildSemaphores()
{
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = NULL;
	semaphoreCreateInfo.flags = 0;

	LOG_ERROR("failed to create present semaphore") <<
	vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &presentComplete);
	LOG_ERROR("failed to create render semaphore") <<
	vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &renderComplete);
	LOG_ERROR("failed to create textoverlay semaphore") <<
	vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &textOverlayComplete);
}

inline VulkanSemaphore::~VulkanSemaphore()
{
	vkDestroySemaphore(m_device, presentComplete, nullptr);
	vkDestroySemaphore(m_device, renderComplete, nullptr);
	vkDestroySemaphore(m_device, textOverlayComplete, nullptr);
}