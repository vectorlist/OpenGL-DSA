#pragma once

#include <vkrenderer.h>

class QWindow;
class Shader;
class Texture;
class TextureRenderer : public VkRenderer
{
public:
	TextureRenderer(QWindow* window);
	virtual~TextureRenderer();

	VkPipelineLayout m_pipelineLayout;
	VkDescriptorSet m_descriptorSet;
	VkDescriptorSetLayout m_descriptorSetLayout;
	VkDescriptorPool m_descriptorPool;

	VkGraphicsPipelineCreateInfo pipelineInfo{};

	void buildProcedural();
	void buildScene();
	void buildDescriptorSetLayout();
	void buildPipeline();
	void buildDescriptorPool();
	void buildDescriptorSet();
	void buildCommandBuffers();

	//texture
	Texture* m_texture;
	void buildTexture();
	
	void render();
	void updateUniformBuffers();
	/*void updateShader();*/
	int count = 0;

	void updateShader();
	bool stageindex = false;
	VkPipeline fixPipeline;
private:
	VkPrimitiveTopology defaultTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
};

