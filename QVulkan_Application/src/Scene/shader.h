#pragma once

#include <vulkan/vulkan.h>
#include <fstream>
#include <ostream>
#include <vktools.h>
#include <memory>

class Shader
{
public:
	Shader(VkDevice device);
	~Shader();
	VkDevice m_device = VK_NULL_HANDLE;

	std::vector<VkShaderModule> shaderModules;
	std::vector<VkPipelineShaderStageCreateInfo> shaderStage;

	void buildSPV(const std::string &vert, const std::string &frag);
	void buildGLSL(const std::string &vert, const std::string &frag);


private:
	void loadSPV(
		const std::string &filename, VkShaderStageFlagBits stage);
	void loadGLSL(
		const std::string &filename, VkShaderStageFlagBits stage);
	std::string codeFromFile(const char* filename);
	void release();
};

typedef std::shared_ptr<Shader> shader_ptr;

