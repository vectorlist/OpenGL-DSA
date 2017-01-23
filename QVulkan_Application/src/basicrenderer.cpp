#include "basicrenderer.h"
#include <qwindow.h>
#include <vktools.h>
#include <vkinitializer.h>
#include <shader.h>
#include <scene.h>
#include <mesh.h>
#include <vertex.h>
#include <vkswapchain.h>

BasicRenderer::BasicRenderer(QWindow* window)
	: VkRenderer(window), m_scene(NULL), m_shader(NULL)
{
	
}


BasicRenderer::~BasicRenderer()
{
	//sub deleter (mesh, shader, descriptor, pipeline..)
	
	vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);

	vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
	vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, nullptr);

	SAFE_DELETE(m_scene);
	SAFE_DELETE(m_shader);
}

void BasicRenderer::buildProcedural()
{
	VkRenderer::buildProcedural();
	m_shader = new Shader(m_device);
	buildScene();					//to create vertex and unifrom buffer
	//m_scene->tuUpdate();

	buildDescriptorSetLayout();
	buildPipeline();			
	buildDescriptorPool();
	buildDescriptorSet();
	buildCommandBuffers();

	//test 
	/*m_scene->tuUpdate();*/

	isBuilt = true;
}

void BasicRenderer::buildScene()
{
	m_scene = new Scene(this);
	
	mesh_ptr mesh = mesh_ptr(new MeshObject);
	meshTool::LoadModel("./model/sphinxfixed.obj", mesh.get());

	shader_ptr shader = shader_ptr(new Shader(m_device));
	shader->buildShader("./shader/vert.spv", "./shader/frag.spv");

	m_scene->addElement(mesh);
	m_scene->addElement(shader);

	m_scene->buildVertexBuffer();
	m_scene->buildIndiceBuffer();
	m_scene->initUniformBuffer();
	m_scene->buildInputState();

	m_scene->updateUnifomrBuffers();
}

void BasicRenderer::buildDescriptorPool()
{
	LOG_SECTION("create descriptor pool");
	std::array<VkDescriptorPoolSize, 2> poolSize = {};
	poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize[0].descriptorCount = 1;
	poolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSize[1].descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = poolSize.size();
	poolInfo.pPoolSizes = poolSize.data();
	poolInfo.maxSets = 1;
	LOG << "descriptor pool size" << poolSize.size() << ENDL;

	LOG_ERROR("failed to create descriptor pool") <<
	vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool);
}

void BasicRenderer::buildDescriptorSet()
{
	LOG_SECTION("update descriptor set");
	VkDescriptorSetLayout layout[] = { m_descriptorSetLayout };
	VkDescriptorSetAllocateInfo allocateinfo{};
	allocateinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateinfo.descriptorPool = m_descriptorPool;
	allocateinfo.descriptorSetCount = 1;
	allocateinfo.pSetLayouts = layout;

	LOG_ERROR("failed to allocate descriptor set") <<
	vkAllocateDescriptorSets(m_device, &allocateinfo, &m_descriptorSet);

	//create buffers
	/*VkDescriptorBufferInfo bufferinfo{};
	bufferinfo.buffer = m_scene->ubo.buffer;
	bufferinfo.offset = 0;
	bufferinfo.range = sizeof(UBODataType);*/

	VkDescriptorImageInfo imageInfo{};

	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = NULL;				//null for atm
	imageInfo.sampler = NULL;				//yet implement

	//std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
	////uniform
	//descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	//descriptorWrites[0].dstSet = m_descriptorSet;
	//descriptorWrites[0].dstBinding = 0;
	//descriptorWrites[0].dstArrayElement = 0;
	//descriptorWrites[0].descriptorCount = 1;
	//descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	//descriptorWrites[0].pBufferInfo = &bufferinfo;
	////sampler
	//descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	//descriptorWrites[1].dstSet = m_descriptorSet;
	//descriptorWrites[1].dstBinding = 1;
	//descriptorWrites[1].dstArrayElement = 0;
	//descriptorWrites[1].descriptorCount = 1;
	//descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	//descriptorWrites[1].pImageInfo = &imageInfo;

	//fixlater for array descriptors

	VkDescriptorBufferInfo bufferinfo{};
	bufferinfo.buffer = m_scene->ubo.buffer;
	bufferinfo.offset = 0;
	bufferinfo.range = sizeof(UBODataType);

	VkWriteDescriptorSet descriptorWrites{};
	//uniform
	descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites.dstSet = m_descriptorSet;
	descriptorWrites.dstBinding = 0;
	descriptorWrites.dstArrayElement = 0;
	descriptorWrites.descriptorCount = 1;
	descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites.pBufferInfo = &bufferinfo;
	
	vkUpdateDescriptorSets(m_device, 1, &descriptorWrites,
		0, nullptr);

	//we need biding this descriptor before rendering
}

void BasicRenderer::updateUniformBuffers()
{
	m_scene->updateUnifomrBuffers();
}

void BasicRenderer::render()
{
	//opengl style
	VkRenderer::begin();

	m_submitInfo.commandBufferCount = 1;
	m_submitInfo.pCommandBuffers = &m_commandBuffers[m_currentBuffer];

	vkQueueSubmit(m_queue, 1, &m_submitInfo, VK_NULL_HANDLE);

	VkRenderer::end();
}


void BasicRenderer::buildCommandBuffers()
{
	VkCommandBufferBeginInfo cmdBufInfo = vkInitializer::commandBufferBeginInfo();

	VkClearValue clearValues[2];
	clearValues[0].color = defaultClearColor;
	clearValues[1].depthStencil = { 1.0f,0 };

	VkRenderPassBeginInfo renderPassBeginInfo = vkInitializer::renderPassBeginInfo();
	renderPassBeginInfo.renderPass = m_renderPass;
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent = { width,height };
	renderPassBeginInfo.clearValueCount = 2;
	renderPassBeginInfo.pClearValues = clearValues;

	for (uint32_t i = 0; i < m_commandBuffers.size(); ++i)
	{
		renderPassBeginInfo.framebuffer = m_frameBuffers[i];
		
		vkBeginCommandBuffer(m_commandBuffers[i], &cmdBufInfo);
		vkCmdBeginRenderPass(m_commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport = vkInitializer::viewport((float)width, (float)height,
			0.0f, 1.0f);
		vkCmdSetViewport(m_commandBuffers[i], 0, 1, &viewport);

		VkRect2D scissor = vkInitializer::rect2D(width, height, 0, 0);
		vkCmdSetScissor(m_commandBuffers[i], 0, 1, &scissor);

		vkCmdBindDescriptorSets(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout, 0, 1, &m_descriptorSet, 0, NULL);

		for (auto& mesh : m_scene->meshs)
		{
			mesh->render(m_commandBuffers[i]);
		}

		vkCmdEndRenderPass(m_commandBuffers[i]);

		vkEndCommandBuffer(m_commandBuffers[i]);
	}

}


void BasicRenderer::buildDescriptorSetLayout()
{
	LOG_SECTION("create descriptor set layout and pipeline layout");
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
		uboLayoutBinding, samplerLayoutBinding
	};

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = bindings.size();
	layoutInfo.pBindings = bindings.data();

	LOG_ERROR("failed to create descriptor set layouts") <<
	vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, &m_descriptorSetLayout);

	/*CREATE PIPELINE LAYOUT*/
	VkPipelineLayoutCreateInfo pPipelineLayoutCreateinfo{};
	pPipelineLayoutCreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pPipelineLayoutCreateinfo.pNext = NULL;
	pPipelineLayoutCreateinfo.setLayoutCount = 1;
	pPipelineLayoutCreateinfo.pSetLayouts = &m_descriptorSetLayout;

	LOG_ERROR("failed to create pipeline layout") <<
	vkCreatePipelineLayout(m_device, &pPipelineLayoutCreateinfo, nullptr, &m_pipelineLayout);
}

void BasicRenderer::buildPipeline()
{
	/*VkShaderModule vertModule = m_shader->loadShader("./shader/vert.spv",
		m_device, VK_SHADER_STAGE_VERTEX_BIT);
	VkShaderModule fragModule = m_shader->loadShader("./shader/frag.spv",
		m_device, VK_SHADER_STAGE_FRAGMENT_BIT);

	VkPipelineShaderStageCreateInfo vertStageInfo{};
	vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertStageInfo.module = vertModule;
	vertStageInfo.pName = "main";


	VkPipelineShaderStageCreateInfo fragStageInfo{};
	fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragStageInfo.module = fragModule;
	fragStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertStageInfo, fragStageInfo };*/

	//vertex
	VkPipelineVertexInputStateCreateInfo vertexInputinfo{};
	vertexInputinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	//we need vertex type infomation
	//it's must be dynamic 
	auto bindingDescrib = Vertex::getBindingDescribtion();
	auto attachDescirb = Vertex::getAttributeDescribtions();

	vertexInputinfo.vertexBindingDescriptionCount = 1;
	vertexInputinfo.pVertexBindingDescriptions = &bindingDescrib;

	vertexInputinfo.vertexAttributeDescriptionCount = attachDescirb.size();
	vertexInputinfo.pVertexAttributeDescriptions = attachDescirb.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = defaultTopology;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = width;
	viewport.height = height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0,0 };
	scissor.extent = { width, height };

	VkPipelineViewportStateCreateInfo viewportStage = {};
	viewportStage.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStage.viewportCount = 1;
	viewportStage.pViewports = &viewport;
	viewportStage.scissorCount = 1;
	viewportStage.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterinfo{};
	rasterinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterinfo.depthClampEnable = VK_FALSE;
	rasterinfo.rasterizerDiscardEnable = VK_FALSE;
	rasterinfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterinfo.lineWidth = 1.0f;
	rasterinfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterinfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; //draw for right hand maybe
	rasterinfo.depthBiasEnable = VK_FALSE;

	//if has multisampling
	VkPipelineMultisampleStateCreateInfo multiSmapling{};
	multiSmapling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multiSmapling.sampleShadingEnable = VK_FALSE;
	multiSmapling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	
	//depth
	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS; //same as GL_DEPTH_LESS
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;	//same as GL_ENABLE_DEPTH_TEST

	VkPipelineColorBlendAttachmentState colorBlendattachment{};
	colorBlendattachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | 
		VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendattachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlendState{};
	colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendState.logicOpEnable = VK_FALSE;
	colorBlendState.logicOp = VK_LOGIC_OP_COPY;
	colorBlendState.attachmentCount = 1;
	colorBlendState.pAttachments = &colorBlendattachment;
	colorBlendState.blendConstants[0] = 0.0f;
	colorBlendState.blendConstants[1] = 0.0f;
	colorBlendState.blendConstants[2] = 0.0f;
	colorBlendState.blendConstants[3] = 0.0f;
	
	//descriptor set layout
	VkDescriptorSetLayout pSetLayouts[] = { m_descriptorSetLayout };
	
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = pSetLayouts;

	LOG_ERROR("failed to create pipeline layout") <<
	vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout);

	std::vector<VkDynamicState> dynamicStateEnables{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.pDynamicStates = dynamicStateEnables.data();
	dynamicState.dynamicStateCount = (uint32_t)dynamicStateEnables.size();
	
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	/*pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;*/
	pipelineInfo.pVertexInputState = &vertexInputinfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportStage;
	pipelineInfo.pRasterizationState = &rasterinfo;
	pipelineInfo.pMultisampleState = &multiSmapling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlendState;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = m_pipelineLayout;
	pipelineInfo.renderPass = m_renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	
	/*SHADER SECTION*/
	//test
	pipelineInfo.stageCount = m_scene->shaders[0]->stageCreateInfos.size();
	pipelineInfo.pStages = m_scene->shaders[0]->stageCreateInfos.data();

	LOG_ERROR("failed to create mesh graphic pipeline") <<
		vkCreateGraphicsPipelines(m_device, m_pipelineCache, 1, &pipelineInfo, nullptr, 
			&m_scene->meshs[0]->pipeline);

}
