
#pragma once

#include <vulkan/vulkan.h>
#include <Windows.h>
#include <vector>
#include <io.h>
#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include <array>
#include <set>
#include <limits>
#include <keycode.h>
#include <chrono>

#include <unordered_map>
#include <vertex.h>
#include <vklog.h>

// Custom define for better code readability
#define VK_FLAGS_NONE 0
// Default fence timeout in nanoseconds
#define DEFAULT_FENCE_TIMEOUT 100000000000

#ifdef __cplusplus
#define VK_UINT32_MAX std::numeric_limits<unsigned int>::max()
//#define VK_UINT64_MAX std::numeric_limits<uint64_t>::max()
#define VK_UINT64_MAX UINT64_MAX
#else
#define VK_UINT32_MAX UINT32_MAX
#define VK_UINT64_MAX UINT64_MAX
#endif



#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<const char*> validationLayers = {
	"VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

extern VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj,
	size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData);


namespace vkDebug
{
	VkResult CreateDebugReportCallbackEXT(
		VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback);

	void DestroyDebugReportCallbackEXT(
		VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator);

}


struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

//uniform buffer object
//struct UBO
//{
//	Matrix4x4 model;
//	Matrix4x4 view;
//	Matrix4x4 proj;
//};

namespace vkTool
{
	std::vector<const char*> getRequiredExtenstions();
	std::vector<char> readfile(const std::string &filename);
	std::array<VkClearValue, 2> defalutClearValue();
	
}


//namespace vkTexture()
