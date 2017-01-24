#include <shader.h>

Shader::Shader(VkDevice device) : m_device(device)
{

}

Shader::~Shader()
{
	release();
}


void Shader::buildSPV(const std::string & vert, const std::string & frag)
{
	if (shaderModules.size() != NULL) {
		LOG_WARN("shader has module already refresh all modules");
		release();
	}
	loadSPV(vert, VK_SHADER_STAGE_VERTEX_BIT);
	loadSPV(frag, VK_SHADER_STAGE_FRAGMENT_BIT);
}

void Shader::buildGLSL(const std::string &vert, const std::string &frag)
{
	if (shaderModules.size() != NULL) {
		LOG_WARN("shader has module already refresh all modules");
		release();
	}
	LOG << "incompleted" << ENDL;
	//loadGLSL(vert, VK_SHADER_STAGE_VERTEX_BIT);
	//loadGLSL(frag, VK_SHADER_STAGE_FRAGMENT_BIT);
}

void Shader::loadSPV(const std::string &filename, VkShaderStageFlagBits stage)
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

		VkPipelineShaderStageCreateInfo stageCreateinfo{};
		stageCreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageCreateinfo.stage = stage;
		stageCreateinfo.module = shaderModule;
		stageCreateinfo.pName = "main";

		shaderModules.push_back(shaderModule);
		shaderStage.push_back(stageCreateinfo);
	}
}

void Shader::loadGLSL(const std::string &filename, VkShaderStageFlagBits stage)
{
	std::string code = codeFromFile(filename.c_str());
	const char* constCode = code.c_str();
	size_t size = strlen(constCode);
	//if (size == 0) LOG_ASSERT("failed to load GLSL code");
	Q_ASSERT(size > 0);
	VkShaderModule shaderModule;
	VkShaderModuleCreateInfo moduleCreateInfo;
	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.pNext = NULL;
	moduleCreateInfo.codeSize = 3 * sizeof(uint32_t) + size + 1;
	moduleCreateInfo.pCode = (uint32_t*)malloc(moduleCreateInfo.codeSize);
	moduleCreateInfo.flags = 0;

	((uint32_t*)moduleCreateInfo.pCode)[0] = 0x07230203;
	((uint32_t*)moduleCreateInfo.pCode)[1] = 0;
	((uint32_t*)moduleCreateInfo.pCode)[2] = stage;
	memcpy(((uint32_t*)moduleCreateInfo.pCode + 3), constCode, size + 1);
	LOG_ERROR("failed to create GLSL") <<
		vkCreateShaderModule(m_device, &moduleCreateInfo, nullptr, &shaderModule);

	VkPipelineShaderStageCreateInfo stageCreateinfo{};
	stageCreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageCreateinfo.stage = stage;
	stageCreateinfo.module = shaderModule;
	stageCreateinfo.pName = "main";

	shaderModules.push_back(shaderModule);
	shaderStage.push_back(stageCreateinfo);
}


std::string Shader::codeFromFile(const char* filename)
{
	std::string bits;
	std::ifstream filestream(filename, std::ios::in);
	if (!filestream.is_open()) {
		LOG_WARN("cound not find file name");
		return "";
	}
	std::string line = "";
	while (!filestream.eof())
	{
		std::getline(filestream, line);
		bits.append(line + "\n");
	}
	filestream.close();
	return bits;
}

void Shader::release()
{
	for (auto module : shaderModules)
	{
		vkDestroyShaderModule(m_device, module, nullptr);
		module = VK_NULL_HANDLE;
	}
	shaderModules.clear();
	shaderStage.clear();
}
