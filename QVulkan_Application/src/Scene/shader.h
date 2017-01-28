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
	VkDevice m_device = VK_NULL_HANDLE;

	//std::array<VkShaderModule, 2> shaderModules;
	/*VkShaderModule vertModule = VK_NULL_HANDLE;
	VkShaderModule fragModule = VK_NULL_HANDLE;*/
	std::vector<VkShaderModule> shaderModules;
	std::vector<VkPipelineShaderStageCreateInfo> shaderStage;

	//there is two option
	void buildSPV(const std::string &vert, const std::string &frag);
	void buildGLSL(const std::string &vert, const std::string &frag);




	/*VkShaderModule loadShaderGLSL(const char *fileName,
		VkDevice device, VkShaderStageFlagBits stage);

	std::string readTextFile(const char *fileName);*/

private:
	void loadSPV(
		const std::string &filename, VkShaderStageFlagBits stage);

	void loadGLSL(
		const std::string &filename, VkShaderStageFlagBits stage);

	std::string codeFromFile(const char* filename);

	void release();
};

typedef std::shared_ptr<Shader> shader_ptr;

namespace shaderTool
{
	VkShaderModule loadShaderGLSL(const char *fileName,
		VkDevice device, VkShaderStageFlagBits stage);

	std::string readTextFile(const char *fileName);
}