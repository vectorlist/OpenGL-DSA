#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class QWindow;
class VulkanInstance;
class VulkanDevice;
class VulkanSwapchain;
class VulkanSemaphore;
class VkRenderer
{
public:
	VkRenderer(QWindow *window);
	virtual~VkRenderer();

	QWindow* m_window;
	HWND m_nativeWindow;

	void initialize();
	/*OVERRIDE*/
	virtual void buildProcedural();
	virtual void render() = 0;
	virtual void updateUniformBuffers() = 0;

	bool isBuilt = false;

	/*COMMON FORMAT*/
	VkFormat m_colorFormat = VK_FORMAT_B8G8R8A8_UNORM;		//vec3
	VkFormat m_depthFormat;

	/*INSTANCE*/
	VulkanInstance* m_vulkanInstance = NULL;
	VkInstance m_instance = VK_NULL_HANDLE;
	VkSurfaceKHR m_surface = VK_NULL_HANDLE;

	/*DEVICE*/
	VulkanDevice* m_vulkanDevice = NULL;
	VkDevice m_device;
	VkPhysicalDevice m_physicalDevice;
	VkPhysicalDeviceFeatures enabledFeatures = {};
	VkQueue m_queue;

	/*SWAPCHAIN*/
	VulkanSwapchain* m_swapchain = NULL;
	uint32_t width;
	uint32_t height;

	/*SEMAPHORE*/
	VulkanSemaphore* m_semaphores = NULL;

	/*SUBMIT INFO*/
	VkPipelineStageFlags m_submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo m_submitInfo;
	void buildSubmitInfo();

	/*COMMAND POOL*/
	VkCommandPool m_commandPool = VK_NULL_HANDLE;

	/*COMMAND BUFFERS*/
	std::vector<VkCommandBuffer> m_commandBuffers;

	/*DEPTH STENCIL*/
	struct
	{
		VkImage image;
		VkImageView view;
		VkDeviceMemory memory;
	}m_depthStencil;

	/*REDNER PASS*/
	VkRenderPass m_renderPass;

	/*PIPELINE CACHE*/
	VkPipelineCache m_pipelineCache;

	/*FRAME BUFFERS*/
	std::vector<VkFramebuffer> m_frameBuffers;
	uint32_t m_currentBuffer = 0;				//active frame buffer index

	VkClearColorValue defaultClearColor = { { 0.125f, 0.125f, 0.125f, 1.0f } };
	
	/*SUB FUNCTIONS*/
	void buildCommandPool();
	void allocateCommandBuffers();
	void buildDepthStencil();
	void buildRenderPass();
	void buildPipelineCache();
	void buildFrameBuffer();

	/*ADDITIONAL FUNCTIONS*/
	void buildCommandBuffers() {};

	//prepare render function
	void begin();				//frame
	void end();					//sumit

	
};

