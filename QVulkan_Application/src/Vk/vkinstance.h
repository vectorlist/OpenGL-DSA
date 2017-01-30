#pragma once

#include <vkTools.h>

class QWindow;
class VulkanInstance
{
public:
	VulkanInstance(VkInstance &instance, VkSurfaceKHR &surface, bool enablevalidation = true);
	~VulkanInstance();

	void buildLayers();
	void buildInstance();
	void buildDebug();
	void buildSurface(HWND nativeWindow);

	bool enableValidation;
	std::vector<const char*> m_layers;
	std::vector<const char*> m_extensions;
	VkInstance &m_instance /*= VK_NULL_HANDLE*/;
	VkDebugReportCallbackEXT m_callback = VK_NULL_HANDLE;
	VkSurfaceKHR &m_surface;
};

