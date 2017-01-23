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

	void render();

	void buildProcedural();


	//member
	VkPipelineLayout m_pipelineLayout;
	VkDescriptorSet m_descriptorSet;
	VkDescriptorSetLayout m_descriptorSetLayout;
	//VkPipeline m_pipeline;
	
	
	Shader* m_shader;
	
	Scene *m_scene;
	//at last
	void buildCommandBuffers();

	void buildDescriptorSetLayout();
	void buildPipeline();

	void buildScene();

	/*DESCRIPTOR*/
	void buildDescriptorPool();
	void buildDescriptorSet();
	VkDescriptorPool m_descriptorPool;

	void updateUniformBuffers();
private:
	VkPrimitiveTopology defaultTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
};

