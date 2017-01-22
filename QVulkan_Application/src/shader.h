#pragma once

#include <vulkan/vulkan.h>
#include <fstream>
#include <ostream>
#include <vktools.h>

class Shader
{
public:
	Shader(VkDevice &device) : m_device(device) {};
	//ref 
	VkDevice &m_device;

	std::vector<VkShaderModule> m_shaderModules;

	inline VkShaderModule loadShader(
		const std::string &filename, VkDevice device, VkShaderStageFlags flag)
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
			vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &shaderModule);

			delete[] code;

			return shaderModule;
		}
	}

	inline VkPipelineShaderStageCreateInfo loadShaderStage(
		const std::string &filename, VkShaderStageFlagBits stageFlag)
	{
		VkPipelineShaderStageCreateInfo shaderStage{};
		shaderStage.stage = stageFlag;
		shaderStage.module = loadShader(filename.c_str(), m_device, stageFlag);
		shaderStage.pName = "main";

		if (shaderStage.module == NULL) LOG_ASSERT("failed to create module");
		m_shaderModules.push_back(shaderStage.module);

		return shaderStage;
	}
};