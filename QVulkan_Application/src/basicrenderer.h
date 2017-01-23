#pragma once

#include <vkrenderer.h>

class QWindow;
class Shader;
class Scene;
class BasicRenderer : public VkRenderer
{
public:
	BasicRenderer(QWindow* window);
	virtual~BasicRenderer();

	VkPipelineLayout m_pipelineLayout;
	VkDescriptorSet m_descriptorSet;
	VkDescriptorSetLayout m_descriptorSetLayout;
	VkDescriptorPool m_descriptorPool;

	Scene *m_scene;
	void buildProcedural();
	void buildScene();
	void buildDescriptorSetLayout();
	void buildPipeline();
	void buildDescriptorPool();
	void buildDescriptorSet();
	void buildCommandBuffers();
	
	void render();
	void updateUniformBuffers();
private:
	VkPrimitiveTopology defaultTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
};

