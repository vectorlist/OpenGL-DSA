#include "pipelineinfo.h"

PipelineInfo::PipelineInfo()
{
}


PipelineInfo::~PipelineInfo()
{
}

void PipelineInfo::initialize(uint32_t width, uint32_t height)
{
	/*INPUT ASSEMBLY*/
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	/*VIEWPORT*/
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = (float)width;
	viewport.height = (float)height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissor.offset = { 0,0 };
	scissor.extent = { width, height };

	/*VIEWPORT STATE*/
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	/*RASTER STATE*/
	rasterState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterState.depthClampEnable = VK_FALSE;
	rasterState.rasterizerDiscardEnable = VK_FALSE;
	rasterState.polygonMode = VK_POLYGON_MODE_FILL;
	rasterState.lineWidth = 1.0f;
	rasterState.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; 
	rasterState.depthBiasEnable = VK_FALSE;

	/*NULTISAMPLING STATE*/
	multiSamplingState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multiSamplingState.sampleShadingEnable = VK_FALSE;
	multiSamplingState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	/*DEPTHSTENCIL STATE*/
	depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilState.depthTestEnable = VK_TRUE;
	depthStencilState.depthWriteEnable = VK_TRUE;
	depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL; 
	depthStencilState.depthBoundsTestEnable = VK_FALSE;
	depthStencilState.stencilTestEnable = VK_FALSE;	

	/*COLOR BLEND ATTACHMENTS*/
	//we can't handle attachment
	colorBlendattachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendattachment.blendEnable = VK_FALSE;

	/*COLOR BLEND STATE*/
	colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendState.logicOpEnable = VK_FALSE;
	colorBlendState.logicOp = VK_LOGIC_OP_COPY;
	colorBlendState.attachmentCount = 1;
	colorBlendState.pAttachments = &colorBlendattachment;
	colorBlendState.blendConstants[0] = 0.0f;
	colorBlendState.blendConstants[1] = 0.0f;
	colorBlendState.blendConstants[2] = 0.0f;
	colorBlendState.blendConstants[3] = 0.0f;


	dynamicStateEnables = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
		VK_DYNAMIC_STATE_DEPTH_BIAS
	};

	/*DYNAMIC STATE*/
	
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.pDynamicStates = dynamicStateEnables.data();
	dynamicState.dynamicStateCount = (uint32_t)dynamicStateEnables.size();

}


void PipelineInfo::buildPipelineInfo()
{
	graphicsPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	/*------------------------ DEFUALT STATE ---------------------------*/
	graphicsPipelineInfo.pInputAssemblyState = &inputAssembly;
	graphicsPipelineInfo.pViewportState = &viewportState;
	graphicsPipelineInfo.pRasterizationState = &rasterState;
	graphicsPipelineInfo.pMultisampleState = &multiSamplingState;
	graphicsPipelineInfo.pDepthStencilState = &depthStencilState;
	graphicsPipelineInfo.pColorBlendState = &colorBlendState;
	graphicsPipelineInfo.pDynamicState = &dynamicState;

	/*------------------------ OUTSIDE STATE ---------------------------*/

	graphicsPipelineInfo.layout = pipelineLayout;
	graphicsPipelineInfo.renderPass = renderPass;
	graphicsPipelineInfo.pVertexInputState = &vertexInputState;
	graphicsPipelineInfo.stageCount = shaderStages.size(); 
	graphicsPipelineInfo.pStages = shaderStages.data();
	
	/*------------------------------------------------------------------*/

	graphicsPipelineInfo.subpass = 0;
	graphicsPipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
}
