#include "vkrenderer.h"
#include <qwindow.h>
#include <vkinstance.h>
#include <vkdevice.h>
#include <vkswapchain.h>
#include <vksemaphore.h>

VkRenderer::VkRenderer(QWindow *window)
	: m_window(window)
{
	m_nativeWindow = reinterpret_cast<HWND>(window->winId());
	
}


VkRenderer::~VkRenderer()
{
	//core (device , instance, commandbuffer, swapchain) deleter
	m_swapchain->clean();
	SAFE_DELETE(m_swapchain);
	
	vkFreeCommandBuffers(m_device, m_commandPool, m_commandBuffers.size(), m_commandBuffers.data());
	//sub build funtions
	vkDestroyRenderPass(m_device, m_renderPass, nullptr);
	for (uint32_t i = 0; i < m_frameBuffers.size(); i++)
	{
		vkDestroyFramebuffer(m_device, m_frameBuffers[i], nullptr);
	}

	vkDestroyImageView(m_device, m_depthStencil.view, nullptr);
	vkDestroyImage(m_device, m_depthStencil.image, nullptr);
	vkFreeMemory(m_device, m_depthStencil.memory, nullptr);

	vkDestroyPipelineCache(m_device, m_pipelineCache, nullptr);

	if (m_commandPool)
		vkDestroyCommandPool(m_device, m_commandPool, nullptr);

	//initialize Core funtions
	SAFE_DELETE(m_semaphores);
	SAFE_DELETE(m_vulkanDevice);
	SAFE_DELETE(m_vulkanInstance);
}

void VkRenderer::initialize()
{
	//basic core initialize(instance, device, physicaldevice surface)
	m_vulkanInstance = new VulkanInstance(m_instance, m_surface);
	m_vulkanInstance->buildLayers(true);
	m_vulkanInstance->buildInstance();
	m_vulkanInstance->buildDebug(true);
	m_vulkanInstance->buildSurface(m_nativeWindow);

	m_vulkanDevice = new VulkanDevice(m_instance, m_device, m_physicalDevice);
	m_vulkanDevice->buildPhysicalDevice();
	m_vulkanDevice->buildLogicalDevice(enabledFeatures);
	m_vulkanDevice->getGraphicsQueue(&m_queue);					
	m_vulkanDevice->getSupportedDepthFormat(&m_depthFormat);
	
	m_swapchain = new VulkanSwapchain(m_instance, m_device, m_physicalDevice, m_surface);
	m_swapchain->init();

	m_semaphores = new VulkanSemaphore(m_device);
	m_semaphores->buildSemaphores();

	buildSubmitInfo();
}

void VkRenderer::buildProcedural()
{
	//sub
	buildCommandPool();
	m_swapchain->buildSwapchain(&width, &height);
	allocateCommandBuffers();
	buildDepthStencil();
	buildRenderPass();
	buildPipelineCache();
	buildFrameBuffer();

	//missing something
	//isBuilt = true;
}

void VkRenderer::buildSubmitInfo()
{
	m_submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	m_submitInfo.pNext = NULL;
	m_submitInfo.pWaitDstStageMask = &m_submitPipelineStages;
	m_submitInfo.waitSemaphoreCount = 1;
	m_submitInfo.pWaitSemaphores = &m_semaphores->presentComplete;
	m_submitInfo.signalSemaphoreCount = 1;
	m_submitInfo.pSignalSemaphores = &m_semaphores->renderComplete;
}

void VkRenderer::begin()
{
	//fix here for descriptor set
	LOG_RESULT(m_swapchain->acquireNextimage(m_semaphores->presentComplete, &m_currentBuffer));
}

void VkRenderer::end()
{
	LOG_RESULT(m_swapchain->queuePresent(m_queue, m_currentBuffer,m_semaphores->renderComplete));
	LOG_RESULT(vkQueueWaitIdle(m_queue));
}