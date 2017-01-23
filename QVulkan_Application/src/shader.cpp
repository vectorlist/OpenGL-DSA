#include <shader.h>

Shader::Shader(VkDevice device) : m_device(device)
{

}

Shader::~Shader()
{
	/*for (auto module : shaderModules)
	{
		vkDestroyShaderModule(m_device, module, nullptr);
	}*/

	vkDestroyShaderModule(m_device, vertModule, nullptr);
	//vertModule = VK_NULL_HANDLE;
	vkDestroyShaderModule(m_device, fragModule, nullptr);
	//fragModule = VK_NULL_HANDLE;
}


void Shader::buildShader(const std::string & vert, const std::string & frag)
{
	/*CREATE MODULE*/
	vertModule = loadShader(vert, VK_SHADER_STAGE_VERTEX_BIT);
	fragModule = loadShader(frag, VK_SHADER_STAGE_FRAGMENT_BIT);

	/*CREATE STAGE INFO*/
	stageCreateInfos[0] = stageFromModule(vertModule, VK_SHADER_STAGE_VERTEX_BIT);
	stageCreateInfos[1] = stageFromModule(fragModule, VK_SHADER_STAGE_FRAGMENT_BIT);

}

VkShaderModule Shader::loadShader(
	const std::string &filename, VkShaderStageFlags flag)
{
	std::ifstream file(filename, std::ios::binary | std::ios::in | std::ios::ate);

	if (file.is_open())
	{
		size_t size = file.tellg();
		file.seekg(0, std::ios::beg);
		char* code = new char[size];
		file.read(code, size);
		file.close();

		assert(size > 0);

		VkShaderModule shaderModule;
		VkShaderModuleCreateInfo moduleCreateInfo{};
		moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		moduleCreateInfo.codeSize = size;
		moduleCreateInfo.pCode = (uint32_t*)code;

		LOG_ERROR("failed to create shader module") <<
			vkCreateShaderModule(m_device, &moduleCreateInfo, nullptr, &shaderModule);

		delete[] code;

		return shaderModule;
	}
}

VkPipelineShaderStageCreateInfo Shader::stageFromModule(
	VkShaderModule module,
	VkShaderStageFlagBits stage)
{
	VkPipelineShaderStageCreateInfo stageInfo{};
	stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageInfo.stage = stage;
	stageInfo.module = module;
	stageInfo.pName = "main";

	return stageInfo;
}

