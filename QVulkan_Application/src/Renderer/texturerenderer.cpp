#include "TextureRenderer.h"
#include <qwindow.h>
#include <vktools.h>
#include <vkinitializer.h>
#include <scene.h>
#include <mesh.h>
#include <vertex.h>
#include <vkswapchain.h>
#include <texture.h>

TextureRenderer::TextureRenderer(QWindow* window)
	: VkRenderer(window)//, //m_scene(NULL)
{
	//enabledFeatures
}


TextureRenderer::~TextureRenderer()
{

	if (soildPipeline)
		vkDestroyPipeline(m_device, soildPipeline, nullptr);
	if (wirePipeline)
		vkDestroyPipeline(m_device, wirePipeline, nullptr);

	SAFE_DELETE(m_texture);
	SAFE_DELETE(m_scene);

	vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
	m_pipelineLayout = VK_NULL_HANDLE;
	vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
}

void TextureRenderer::buildProcedural()
{
	VkRenderer::initialize();
	VkRenderer::buildProcedural();
	buildScene();
	buildTexture();

	buildDescriptorSetLayout();
	buildPipeline();
	buildDescriptorPool();
	buildDescriptorSet();
	buildCommandBuffers();

	isBuilt = true;
}

void TextureRenderer::buildScene()
{
	LOG_SECTION("build scene");
	m_scene = new Scene(this);

	vkmesh_ptr mesh = vkmesh_ptr(new VKMesh);
	meshTool::LoadModel("./model/stone_f.obj", mesh.get());

	shader_ptr shader = shader_ptr(new Shader(m_device));
	//SPV
	//shader->buildSPV("./shader/texturerenderer/vert.spv", "./shader/texturerenderer/frag.spv");
	shader->buildGLSL("./shader/texturerenderer/shader.vert", "./shader/texturerenderer/shader.frag");
	
	camera_ptr camera = camera_ptr(new Camera);

	m_scene->addElement(camera);
	m_scene->addElement(mesh);
	m_scene->addElement(shader);

	m_scene->buildVertexBuffer();
	m_scene->buildIndiceBuffer();
	m_scene->initUniformBuffer();
	m_scene->buildInputState();

	m_scene->updateUnifomrBuffers();
}


void TextureRenderer::buildDescriptorSetLayout()
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
	/*CREATE DESCRIPTOR LAYOUT*/
	VkDescriptorSetLayoutCreateInfo descritorSetlayoutInfo{};
	descritorSetlayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descritorSetlayoutInfo.bindingCount = bindings.size();
	descritorSetlayoutInfo.pBindings = bindings.data();

	LOG_ERROR("failed to create descriptor set layouts") <<
		vkCreateDescriptorSetLayout(m_device, &descritorSetlayoutInfo, nullptr, &m_descriptorSetLayout);

	/*CREATE PIPELINE LAYOUT*/
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.pNext = NULL;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;

	LOG_ERROR("failed to create pipeline layout") <<
		vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout);
}


void TextureRenderer::buildPipeline()
{
	//SET DEFUALT STATES
	pipelineInfo = pipelineinfo_ptr(new PipelineInfo);
	pipelineInfo->initialize(width, height);

	//SET OUTSIDE STATES(layout, renderpass vertexInput)
	pipelineInfo->pipelineLayout = m_pipelineLayout;
	pipelineInfo->renderPass = m_renderPass;
	pipelineInfo->vertexInputState = m_scene->vertexInputState;

	//SET MAIN SHADER
	mainShader = shader_ptr(new Shader(m_device));
	mainShader->buildGLSL("./shader/default/main.vert", "./shader/default/main.frag");

	pipelineInfo->shaderStages = mainShader->shaderStage;
	pipelineInfo->buildPipelineInfo();

	for (auto &mesh : m_scene->meshs)
	{
		vkCreateGraphicsPipelines(m_device, m_pipelineCache, 1, 
			&pipelineInfo->graphicsPipelineInfo,
			nullptr, &mesh->pipeline);
	}

	//SET SOLID PIPELINE
	auto solidShader = shader_ptr(new Shader(m_device));
	solidShader->buildGLSL("./shader/default/solid.vert", "./shader/default/solid.frag");

	pipelineInfo->shaderStages = solidShader->shaderStage;
	vkCreateGraphicsPipelines(m_device, m_pipelineCache, 1, &pipelineInfo->graphicsPipelineInfo,
		nullptr, &soildPipeline);

	//SET WIRE PIPELINE
	pipelineInfo->rasterState.polygonMode = VK_POLYGON_MODE_LINE;
	pipelineInfo->rasterState.lineWidth = 1.0f;
	pipelineInfo->rasterState.depthBiasEnable = VK_TRUE;

	auto wireShader = shader_ptr(new Shader(m_device));
	wireShader->buildGLSL("./shader/default/solid.vert", "./shader/default/wire.frag");
	pipelineInfo->shaderStages = wireShader->shaderStage;

	vkCreateGraphicsPipelines(m_device, m_pipelineCache, 1, &pipelineInfo->graphicsPipelineInfo,
		nullptr, &wirePipeline);


}

void TextureRenderer::buildDescriptorPool()
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

void TextureRenderer::buildDescriptorSet()
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

	VkDescriptorImageInfo imageInfo{};

	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = NULL;				//null for atm
	imageInfo.sampler = NULL;				//yet implement

	VkDescriptorBufferInfo bufferinfo{};
	bufferinfo.buffer = m_scene->ubo.buffer;
	bufferinfo.offset = 0;
	bufferinfo.range = sizeof(UBODataType);

	VkWriteDescriptorSet uniformWrites{};
	//vertex shader binding 0
	uniformWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	uniformWrites.dstSet = m_descriptorSet;
	uniformWrites.dstBinding = 0;
	uniformWrites.dstArrayElement = 0;
	uniformWrites.descriptorCount = 1;
	uniformWrites.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformWrites.pBufferInfo = &bufferinfo;

	VkWriteDescriptorSet imageWrites{};
	//frag shader binding 1
	imageWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	imageWrites.dstSet = m_descriptorSet;
	imageWrites.dstBinding = 1;
	imageWrites.dstArrayElement = 0;
	imageWrites.descriptorCount = 1;
	imageWrites.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	imageWrites.pImageInfo = &m_texture->descriptor;

	/*std::vector<VkWriteDescriptorSet> writesDescriptors{
		uniformWrites, imageWrites
	};*/
	std::vector<VkWriteDescriptorSet> writeDescriptors;
	writeDescriptors.push_back(uniformWrites);
	writeDescriptors.push_back(imageWrites);
	vkUpdateDescriptorSets(m_device, writeDescriptors.size(), writeDescriptors.data(),
		0, nullptr);
	/*vkUpdateDescriptorSets(m_device, 1, &descriptorWrites,
		0, nullptr);*/

}

void TextureRenderer::buildCommandBuffers()
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

		renderOptional(m_commandBuffers[i], m_renderType);

		vkCmdEndRenderPass(m_commandBuffers[i]);

		vkEndCommandBuffer(m_commandBuffers[i]);
	}

}

void TextureRenderer::buildTexture()
{
	m_texture = new Texture(m_vulkanDevice);
	m_texture->loadTexture("./image/checker.jpg", VK_FORMAT_R8G8B8A8_UNORM, false);
	//m_texture->loadTexture("./image/checker.jpg", VK_FORMAT_R8G8B8_UNORM, false);
	//m_texture->loadTexture("./image/rock2.jpg", VK_FORMAT_R8G8B8A8_UNORM, false);
}

void TextureRenderer::render()
{
	VkRenderer::begin();

	m_submitInfo.commandBufferCount = 1;
	m_submitInfo.pCommandBuffers = &m_commandBuffers[m_currentBuffer];

	vkQueueSubmit(m_queue, 1, &m_submitInfo, VK_NULL_HANDLE);

	VkRenderer::end();
}

void TextureRenderer::updateUniformBuffers()
{
	m_scene->updateUnifomrBuffers();
}

void TextureRenderer::updateShader()
{
	//test
	if (m_renderType == RenderType::MAIN)
		m_renderType = RenderType::SOILDWIRE;
	else if (m_renderType == RenderType::SOILDWIRE)
			m_renderType = RenderType::WIRE;
	else if (m_renderType == RenderType::WIRE)
			m_renderType = RenderType::MAIN;
	
	rebuildCommandBuffers();
	update();
}


void TextureRenderer::renderOptional(VkCommandBuffer cmd, RenderType type)
{
	for (auto& mesh : m_scene->meshs)
	{
		if (type == RenderType::MAIN)
			mesh->render(cmd, NULL);

		else if (type == RenderType::SOILDWIRE) {
			mesh->render(cmd, soildPipeline);
			//vkCmdSetDepthBias (polygon offset)
			/*commandBuffer is the command buffer into which the command will be recorded.
			depthBiasConstantFactor : is a scalar factor controlling the
				constant depth value added to each fragment.
			depthBiasClamp  : is the maximum(or minimum) depth bias of a fragment.
			depthBiasSlopeFactor : is a scalar factor applied to a fragment¡¯s 
				slope in depth bias calculations.*/
			//set to slope increase clamps negative -1.0f from 0.0f - 1.0f
			vkCmdSetDepthBias(cmd, 1.0f, 0.0f, -1.0f);
			mesh->render(cmd, wirePipeline);
		}
		else if (type == RenderType::WIRE) {
			mesh->render(cmd, wirePipeline);
		}
	}
}