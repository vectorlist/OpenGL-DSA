#include <vkdevice.h>



VulkanDevice::VulkanDevice(VkInstance &instance, VkDevice &device, VkPhysicalDevice &physicalDevice)
	: m_instance(instance), m_device(device), m_physicalDevice(physicalDevice)
{

}


VulkanDevice::~VulkanDevice()
{
	if (m_commandPool) {
		vkDestroyCommandPool(m_device, m_commandPool, nullptr);
	}
	if (m_device)
	{
		vkDestroyDevice(m_device, nullptr);
	}
}

void VulkanDevice::buildPhysicalDevice()
{
	LOG_SECTION("create physical device");
	uint32_t gpuCount = 0;
	vkEnumeratePhysicalDevices(m_instance, &gpuCount, nullptr);
	std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
	LOG_ERROR("failed to find available physical device") << 
	vkEnumeratePhysicalDevices(m_instance, &gpuCount, physicalDevices.data());
	
	m_physicalDevice = physicalDevices[0];

	LOG << "enabled gpu num : " << gpuCount << ENDL;

	vkGetPhysicalDeviceProperties(m_physicalDevice, &m_properties);
	vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_features);
	vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_memoryProperties);

	/*QUEUE FAMILY PROPERTIES*/ 
	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);
	assert(queueFamilyCount > 0);
	m_queueFamilyProperties.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, m_queueFamilyProperties.data());

	/*SUPPORTED EXTENSIONS*/
	uint32_t extCount = 0;
	vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &extCount, nullptr);
	if (extCount == 0) {
		LOG << "physical device has no extensions";
		return;
	}
	std::vector<VkExtensionProperties> extensions(extCount);
	if (vkEnumerateDeviceExtensionProperties(m_physicalDevice,
		nullptr, &extCount, &extensions.front()) == VK_SUCCESS)
	{
		for (auto ext : extensions)
		{
			m_supportedExtensions.push_back(ext.extensionName);
			LOG << "supported device extension : " << ext.extensionName << ENDL;
		}
	}

}

void VulkanDevice::buildLogicalDevice(
	VkPhysicalDeviceFeatures enabledFeatures,
	bool useSwapcahin,
	VkQueueFlags requestedQueueTypes)
{
	LOG_SECTION("create logical device");
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

	// Get queue family indices for the requested queue family types
	// Note that the indices may overlap depending on the implementation

	const float defaultQueuePriority(0.0f);

	// Graphics queue
	if (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT)
	{
		m_queueFamilyIndices.graphics = getQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
		VkDeviceQueueCreateInfo queueInfo{};
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.queueFamilyIndex = m_queueFamilyIndices.graphics;
		queueInfo.queueCount = 1;
		queueInfo.pQueuePriorities = &defaultQueuePriority;
		queueCreateInfos.push_back(queueInfo);
	}
	else
	{
		m_queueFamilyIndices.graphics = VK_NULL_HANDLE;
	}

	// Dedicated compute queue
	if (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT)
	{
		m_queueFamilyIndices.compute = getQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT);
		if (m_queueFamilyIndices.compute != m_queueFamilyIndices.graphics)
		{
			// If compute family index differs, we need an additional queue create info for the compute queue
			VkDeviceQueueCreateInfo queueInfo{};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = m_queueFamilyIndices.compute;
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = &defaultQueuePriority;
			queueCreateInfos.push_back(queueInfo);
		}
	}
	else
	{
		// Else we use the same queue
		m_queueFamilyIndices.compute = m_queueFamilyIndices.graphics;
	}

	// Dedicated transfer queue
	if (requestedQueueTypes & VK_QUEUE_TRANSFER_BIT)
	{
		m_queueFamilyIndices.transfer = getQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT);
		if ((m_queueFamilyIndices.transfer != m_queueFamilyIndices.graphics)
			&& (m_queueFamilyIndices.transfer != m_queueFamilyIndices.compute))
		{
			// If compute family index differs, we need an additional queue create info for the compute queue
			VkDeviceQueueCreateInfo queueInfo{};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = m_queueFamilyIndices.transfer;
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = &defaultQueuePriority;
			queueCreateInfos.push_back(queueInfo);
		}
	}
	else
	{
		// Else we use the same queue
		m_queueFamilyIndices.transfer = m_queueFamilyIndices.graphics;
	}

	// Create the logical device representation
	std::vector<const char*> deviceExtensions;
	if (useSwapcahin)
	{
		// If the device will be used for presenting to a display
		//via a swapchain we need to request the swapchain extension
		deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		deviceExtensions.push_back(VK_NV_GLSL_SHADER_EXTENSION_NAME);
	}

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount =
		static_cast<uint32_t>(queueCreateInfos.size());;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.pEnabledFeatures = &enabledFeatures;
	deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	
	// Enable the debug marker extension if it is present (likely meaning a debugging tool is present)
	if (extensionSupported(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
	{
		deviceExtensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
		//enableDebugMarkers = true;
	}
	for (auto &ext : deviceExtensions)
		LOG << "enable device extensions : " << ext << ENDL;
	if (deviceExtensions.size() > 0)
	{
		deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	}

	LOG_ERROR("failed to create logical device") <<
	vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_device);
		
	// Create a default command pool for graphics command buffers
	m_commandPool = createCommandPool(m_queueFamilyIndices.graphics);
	
}

void VulkanDevice::getGraphicsQueue(VkQueue *queue)
{
	vkGetDeviceQueue(m_device, m_queueFamilyIndices.graphics, 0,queue);
	vkGetDeviceQueue(m_device, m_queueFamilyIndices.graphics, 0, &m_queue);
}

VkBool32 VulkanDevice::getSupportedDepthFormat(VkFormat *depthFormat)
{
	// Since all depth formats may be optional, we need to find a suitable depth format to use
	// Start with the highest precision packed format
	std::vector<VkFormat> depthFormats = {
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM
	};

	for (auto& format : depthFormats)
	{
		VkFormatProperties formatProps;
		vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &formatProps);
		// Format must support depth stencil attachment for optimal tiling
		if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			*depthFormat = format;
			return true;
		}
	}

	return false;
}


void VulkanDevice::createBuffer(
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkBuffer &buffer,
	VkDeviceMemory &bufferMemory,
	VkDeviceSize size
)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	LOG_ERROR("failed to create buffer") <<
	vkCreateBuffer(m_device, &bufferInfo, nullptr, &buffer);

	VkMemoryRequirements memReq = {};
	vkGetBufferMemoryRequirements(m_device, buffer, &memReq);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReq.size;
	allocInfo.memoryTypeIndex = getMemoryType(memReq.memoryTypeBits, properties);

	LOG_ERROR("failed to allocate buffer memory") <<
	vkAllocateMemory(m_device, &allocInfo, nullptr, &bufferMemory);

	LOG_ERROR("failed to bind buffer to memory") <<
	vkBindBufferMemory(m_device, buffer, bufferMemory, 0);
}

void VulkanDevice::copyBuffer(
	VkBuffer srcBuffer,
	VkBuffer dstBuffer,
	VkDeviceSize size
)
{
	VkCommandBufferAllocateInfo cmdAllocateInfo{};
	cmdAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdAllocateInfo.commandPool = m_commandPool;				//using device command ppol
	cmdAllocateInfo.commandBufferCount = 1;

	//to make disposable commandbuffer
	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(m_device, &cmdAllocateInfo, &commandBuffer);

	VkCommandBufferBeginInfo cmdBeginInfo{};
	cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	/*----------- COPY ------------*/
	//we order to command buffer for copy
	vkBeginCommandBuffer(commandBuffer, &cmdBeginInfo);

	VkBufferCopy copyRegion {};
	copyRegion.size = size;

	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);	//for once atm

	//finish order
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	//using graphic queue for this order
	vkQueueSubmit(m_queue, 1, &submitInfo, VK_NULL_HANDLE);
	//wait for ordered stuff
	vkQueueWaitIdle(m_queue); 

	//free allocated command buffer
	vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
}

void VulkanDevice::destroyBuffer(VkBuffer buffer, VkDeviceMemory bufferMemory)
{
	vkDestroyBuffer(m_device, buffer, nullptr);
	vkFreeMemory(m_device, bufferMemory, nullptr);
}