#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

class PipelineInfo
{
public:
	PipelineInfo();
	~PipelineInfo();


	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	VkViewport viewport = {};
	VkRect2D scissor{};

	VkPipelineViewportStateCreateInfo viewportState = {};
	VkPipelineRasterizationStateCreateInfo rasterState{};
	VkPipelineMultisampleStateCreateInfo multiSamplingState{};
	VkPipelineDepthStencilStateCreateInfo depthStencilState{};

	VkPipelineColorBlendAttachmentState colorBlendattachment{};
	VkPipelineColorBlendStateCreateInfo colorBlendState{};

	std::vector<VkDynamicState> dynamicStateEnables;
	VkPipelineDynamicStateCreateInfo dynamicState{};

	VkGraphicsPipelineCreateInfo graphicsPipelineInfo{};

	/*OUTSIDE STATE*/
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkRenderPass renderPass = VK_NULL_HANDLE;
	VkPipelineVertexInputStateCreateInfo vertexInputState{};
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

	
	//init with view port size and defualt PipelineInfo set up
	void initialize(uint32_t width, uint32_t height);
	void buildPipelineInfo();
};

typedef std::shared_ptr<PipelineInfo> pipelineinfo_ptr;

