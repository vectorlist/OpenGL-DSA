#include <vkinstance.h>
#include <iostream>
#include <qwindow.h>


VulkanInstance::VulkanInstance(VkInstance &instance, VkSurfaceKHR &surface)
	: m_instance(instance), m_surface(surface)
{
	
}


VulkanInstance::~VulkanInstance()
{
	if (m_surface) {
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	}
	if (m_callback) {
		vkDebug::DestroyDebugReportCallbackEXT(m_instance, m_callback, nullptr);
	}

	if (m_instance) vkDestroyInstance(m_instance, nullptr);
	m_instance = VK_NULL_HANDLE;
}

void VulkanInstance::buildLayers(bool enableValidation)
{
	/*INSTANCE LAYERS*/
	m_layers.push_back("VK_LAYER_LUNARG_standard_validation");

	/*EXTENTION LAYERS*/
	m_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	m_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
	//m_extensions.push_back(VK_NV_GLSL_SHADER_EXTENSION_NAME);
	//m_extensions.push_back("VK_NV_glsl_shader");			//push back
	//m_extensions.push_back("GL_KHR_vulkan_glsl");
	if (enableValidation)
		m_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

}

void VulkanInstance::buildInstance()
{
	LOG_SECTION("create instance");

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.apiVersion = VK_MAKE_VERSION(1,0,0);

	VkInstanceCreateInfo instanceCreateInfo{};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.enabledLayerCount = (uint32_t)m_layers.size();
	instanceCreateInfo.ppEnabledLayerNames = m_layers.data();
	instanceCreateInfo.enabledExtensionCount = (uint32_t)m_extensions.size();
	instanceCreateInfo.ppEnabledExtensionNames = m_extensions.data();
	//add this?

	LOG_ERROR("failed to create instance") <<
		vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance);

}

void VulkanInstance::buildDebug(bool enableValidation)
{
	if (!enableValidation) return;
	LOG_SECTION("create debug callback funtion");
	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;

	LOG_ERROR("failed to create debug callback funtion") <<
		vkDebug::CreateDebugReportCallbackEXT(m_instance, &createInfo, nullptr, &m_callback);
	
}

void VulkanInstance::buildSurface(HWND nativeWindow)
{
	LOG_SECTION("create vulkan surface");
	VkWin32SurfaceCreateInfoKHR surface_createInfo{};
	surface_createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surface_createInfo.hinstance = GetModuleHandle(NULL);
	surface_createInfo.hwnd = (HWND)nativeWindow;

	LOG_ERROR("failed to create vulkan surfce") << 
		vkCreateWin32SurfaceKHR(m_instance, &surface_createInfo, nullptr, &m_surface);
}
