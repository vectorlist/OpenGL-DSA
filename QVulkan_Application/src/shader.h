#pragma once

#include <vulkan/vulkan.h>
#include <fstream>
#include <ostream>
#include <vktools.h>
#include <memory>

#define SHADER_TYPE_NUM			2
#define VERT_INDEX				0
#define FRAG_INDEX				1

class Shader
{
public:
	Shader(VkDevice device);
	~Shader();
	VkDevice m_device;

	//std::array<VkShaderModule, 2> shaderModules;
	VkShaderModule vertModule = VK_NULL_HANDLE;
	VkShaderModule fragModule = VK_NULL_HANDLE;
	std::array<VkPipelineShaderStageCreateInfo, 2> stageCreateInfos;

	void buildShader(const std::string &vert, const std::string &frag);

private:
	VkShaderModule loadShader(
		const std::string &filename, VkShaderStageFlags flag);


	VkPipelineShaderStageCreateInfo stageFromModule(
		VkShaderModule module, 
		VkShaderStageFlagBits stage);
	
};

typedef std::shared_ptr<Shader> shader_ptr;