#pragma once

#include <vkrenderer.h>
#include <shader.h>
#include <pipelineinfo.h>

enum class RenderType : uint32_t
{
	MAIN = 1 << 1,
	SOILDWIRE = 1 << 2,
	WIRE = 1 << 3,

};

class QWindow;
class Shader;
class Texture;
class Pipeline;
class TextureRenderer : public VkRenderer
{
public:
	TextureRenderer(QWindow* window);
	virtual~TextureRenderer();

	VkPipelineLayout m_pipelineLayout;
	VkDescriptorSet m_descriptorSet;
	VkDescriptorSetLayout m_descriptorSetLayout;
	VkDescriptorPool m_descriptorPool;

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

	void updateShader();
	//test
	void renderOptional(VkCommandBuffer cmd, RenderType type);
	RenderType m_renderType = RenderType::MAIN;

	/*DEFAULT PIPELINES AND SHADER*/
	pipelineinfo_ptr pipelineInfo = NULL;

	VkPipeline soildPipeline;
	VkPipeline wirePipeline;

	
	shader_ptr mainShader = NULL;

	/*shader_ptr solidShader = NULL;
	shader_ptr wireShader = NULL;*/
	
private:
	VkPrimitiveTopology defaultTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
};

