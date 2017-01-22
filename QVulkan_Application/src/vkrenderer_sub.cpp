
#include <vkrenderer.h>
#include <vktools.h>
#include <vkswapchain.h>
#include <vkdevice.h>

	/*SUB BUILD FUNCTIONS*/

void VkRenderer::buildCommandPool()
{
	LOG_SECTION("create command pool");
	VkCommandPoolCreateInfo cmdPoolInfo{};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.queueFamilyIndex - m_swapchain->m_queueNodeIndex;
	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	LOG_ERROR("failed to create command pool") <<
		vkCreateCommandPool(m_device, &cmdPoolInfo, nullptr, &m_commandPool);
}

void VkRenderer::allocateCommandBuffers()
{
	LOG_SECTION("create command buffers");
	m_commandBuffers.resize(m_swapchain->m_imageCount);
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();

	LOG_ERROR("failed to create command buffers") <<
		vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers.data());
}

void VkRenderer::buildDepthStencil()
{
	LOG_SECTION("create depth stencil");
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.pNext = NULL;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.format = m_depthFormat;
	imageInfo.extent = { width , height, 1 };
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	imageInfo.flags = 0;

	VkMemoryAllocateInfo memAllocInfo{};
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAllocInfo.pNext = NULL;
	memAllocInfo.allocationSize = 0;			//default 
	memAllocInfo.memoryTypeIndex = 0;

	VkImageViewCreateInfo depthStencilView{};
	depthStencilView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	depthStencilView.pNext = NULL;
	//depthStencilView.image = ;      //we will attach image after bind
	depthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
	depthStencilView.format = m_depthFormat;
	depthStencilView.flags = 0;
	depthStencilView.subresourceRange = {};
	depthStencilView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT |
		VK_IMAGE_ASPECT_STENCIL_BIT;
	depthStencilView.subresourceRange.baseMipLevel = 0;
	depthStencilView.subresourceRange.levelCount = 1;
	depthStencilView.subresourceRange.baseArrayLayer = 0;
	depthStencilView.subresourceRange.layerCount = 1;

	VkMemoryRequirements memReqs;

	/*CREATE IMAGE*/
	vkCreateImage(m_device, &imageInfo, nullptr, &m_depthStencil.image);
	//get reqiure memory
	vkGetImageMemoryRequirements(m_device, m_depthStencil.image, &memReqs);
	
	//get allocate info from reqiurement memory properties
	memAllocInfo.allocationSize = memReqs.size;			//default 
	memAllocInfo.memoryTypeIndex = m_vulkanDevice->getMemoryType(memReqs.memoryTypeBits,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	LOG << "need depth stencil memory size : "<<memReqs.size << ENDL;
	LOG << "need depth stencil type : " << memReqs.memoryTypeBits << ENDL;
	
	LOG_ERROR("failed to allocate depth memory") <<
	vkAllocateMemory(m_device, &memAllocInfo, nullptr, &m_depthStencil.memory);

	LOG_ERROR("failed to bind depth memory") <<
	vkBindImageMemory(m_device,m_depthStencil.image, m_depthStencil.memory, 0);

	depthStencilView.image = m_depthStencil.image;
	
	/*CREATE IMAGE VIEW*/
	LOG_ERROR("failed to create depth image view") <<
	vkCreateImageView(m_device, &depthStencilView, nullptr, &m_depthStencil.view);
}

void VkRenderer::buildRenderPass()
{
	std::array<VkAttachmentDescription, 2> attachments {};
	/*COLOR ATTACHMENT*/
	attachments[0].format = m_colorFormat;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	/*DEPTH ATTACHMENT*/
	attachments[1].format = m_depthFormat;
	attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorRef{};
	colorRef.attachment = 0;
	colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthRef{};
	depthRef.attachment = 1;
	depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDescription{};
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &colorRef;
	subpassDescription.pDepthStencilAttachment = &depthRef;
	subpassDescription.inputAttachmentCount = 0;
	subpassDescription.pInputAttachments = nullptr;
	subpassDescription.preserveAttachmentCount = 0;
	subpassDescription.pPreserveAttachments = nullptr;
	subpassDescription.pResolveAttachments = nullptr;

	std::array<VkSubpassDependency, 2> dependencies;
	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkRenderPassCreateInfo renderPassinfo{};
	renderPassinfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassinfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassinfo.pAttachments = attachments.data();
	renderPassinfo.subpassCount = 1;
	renderPassinfo.pSubpasses = &subpassDescription;
	renderPassinfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassinfo.pDependencies = dependencies.data();

	LOG_ERROR("failed to create redner pass") <<
		vkCreateRenderPass(m_device, &renderPassinfo, nullptr, &m_renderPass);

}

void VkRenderer::buildPipelineCache()
{
	LOG_SECTION("create pipeline caches");
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo{};
	pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

	LOG_ERROR("failed to create pipeline caches") <<
	vkCreatePipelineCache(m_device, &pipelineCacheCreateInfo, nullptr, &m_pipelineCache);
}

void VkRenderer::buildFrameBuffer()
{
	VkImageView attachments[2];

	//depth/ stencil attachment is the same for all frame buffers
	attachments[1] = m_depthStencil.view;

	VkFramebufferCreateInfo frameBufferCreateinfo{};
	frameBufferCreateinfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCreateinfo.pNext = NULL;
	frameBufferCreateinfo.renderPass = m_renderPass;
	frameBufferCreateinfo.attachmentCount = 2;
	frameBufferCreateinfo.pAttachments = attachments;
	frameBufferCreateinfo.width = width;
	frameBufferCreateinfo.height = height;
	frameBufferCreateinfo.layers = 1;

	//create frambuffer for every swap chain image
	m_frameBuffers.resize(m_swapchain->m_imageCount);
	for (uint32_t i = 0; i < m_frameBuffers.size(); ++i)
	{
		//set view index 0
		attachments[0] = m_swapchain->m_buffers[i].view;
		//create frame buffer
		std::string bits = "failed to create frame buffer : ";
		bits.append(std::to_string(i));

		LOG_ERROR(bits.c_str()) <<
		vkCreateFramebuffer(m_device, &frameBufferCreateinfo, nullptr, &m_frameBuffers[i]);
	}

}
