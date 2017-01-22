#include <glshader.h>
#include <string>
#include <vklog.h>
#include <qdebug.h>

GLShader::GLShader() {
	//program = glCreateProgram();
}

GLShader::~GLShader()
{
	
}

void GLShader::buildShader(const std::string &vert, const std::string &frag)
{
	program = glCreateProgram();

	std::string vertSource = codeFromFile(vert);
	std::string fragSource = codeFromFile(frag);

	shader[VERT] = createShader(vertSource, GL_VERTEX_SHADER);
	shader[FRAG] = createShader(fragSource, GL_FRAGMENT_SHADER);
	for (uint32_t i = 0; i < NUM_SHADER; ++i)
		glAttachShader(program, shader[i]);

	glLinkProgram(program);
	debugCompile(program, GL_LINK_STATUS, "error linking the program", true);

	glValidateProgram(program);
	debugCompile(program, GL_VALIDATE_STATUS, "invalid shader program", true);
}

//need to fix return byte code directly
std::string GLShader::codeFromFile(const std::string filename)
{
	std::string bits;
	std::ifstream filestream(filename, std::ios::in);
	if (!filestream.is_open()) {
		LOG_WARN("failed to load file : " + filename);
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

GLuint GLShader::createShader(const std::string &srcCode, GLuint type)
{
	GLuint shader = glCreateShader(type);
	if (shader == 0)
		LOG_ASSERT("error compiling shader type");
	const char* code = srcCode.c_str();

	glShaderSource(shader, 1, &code, NULL);
	glCompileShader(shader);

	debugCompile(shader, GL_COMPILE_STATUS, "error compiling shader");

	return shader;
}

void GLShader::debugCompile(GLuint shader, GLuint flag,
	const std::string &msg, bool isProgram)
{
	GLint sucess = 0;
	GLchar error[1024] = { 0 };

	if (isProgram)
		glGetProgramiv(shader, flag, &sucess);
	else
		glGetShaderiv(shader, flag, &sucess);

	if (sucess == GL_FALSE)
	{
		if (isProgram)
			glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		else
			glGetShaderInfoLog(shader, sizeof(error), NULL, error);
		/*LOG << msg << " : " << error << "'" << ENDL;*/
		qDebug() << msg.c_str() << " : " << error << "'" ;
	}
}

void GLShader::setUniform1i(int data, const char* name)
{
	glUniform1i(glGetUniformLocation(program, name), data);
}

void GLShader::setUuiform1f(float data, const char* name)
{
	glUniform1f(glGetUniformLocation(program, name), data);
}

void GLShader::setUniform2i(const vec2i &data, const char* name)
{
	glUniform2i(glGetUniformLocation(program, name), data.x, data.y);
}

void GLShader::setUniform2f(const vec2f &data, const char* name)
{
	glUniform2f(glGetUniformLocation(program, name), data.x, data.y);
}

void GLShader::setUniform3f(const vec3f &data, const char* name)
{
	glUniform3f(glGetUniformLocation(program, name), data.x,data.y,data.z);
}

void GLShader::setUniformMatrix4f(const Matrix4x4 &mat, const char* name, bool transpose)
{
	glUniformMatrix4fv(glGetUniformLocation(program, name), 1, transpose, mat.constData());
}

// void GLShader::setUniform(const char* name, int value) {
	//	glUniform1i(getLocation(name), value);
	//}
	//void GLShader::setUniform(const char* name, float value) {
	//	glUniform1f(getLocation(name), value);
//void GLShader::attachShader(const char* fileName, uint type) {
//	//std::fstream file(fileName);
//	//std::string fileContents = "";
//
//	////Loading file into string
//	//if (file.is_open()) {
//	//	std::string line;
//	//	while (std::getline(file, line)) {
//	//		fileContents += line + '\n';
//	//	}
//	//	file.close();
//	//}
//	//else {
//	//	PRINT_ERROR << "Could not load " << fileName << "!\n";
//	//	return;
//	//}
//
//	const GLchar* text = fileContents.c_str();
//	GLint length = fileContents.length();
//
//	GLuint shader = glCreateShader(type);
//	glShaderSource(shader, 1, &text, &length);
//	glCompileShader(shader);
//
//	GLint isCompiled = 0;
//	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
//	if (isCompiled == GL_FALSE) {
//		GLint maxLength = 0;
//		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
//
//		char* errorLog = new char[maxLength];
//		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
//		PRINT_ERROR << "Shader failed to compile with message:\n" << errorLog << '\n';
//		delete errorLog;
//
//		glDeleteShader(shader);
//		return;
//	}
//
//	glAttachShader(program, shader);
//
//	glBindAttribLocation(program, 0, "position");
//
//	glLinkProgram(program);
//
//	GLint isLinked = 0;
//	glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
//	if (isLinked == GL_FALSE) {
//		GLint maxLength = 0;
//		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
//
//		char* infoLog = new char[maxLength];
//		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
//		PRINT_ERROR << "Program failed to link with message:\n" << infoLog << '\n';
//		delete infoLog;
//
//		glDeleteShader(shader);
//		return;
//	}
//
//	shaders.push_back(shader);
//}
//
//void GLShader::use() const {
//	glUseProgram(program);
//}
//
//int GLShader::getLocation(const char* name) {
//	const auto loc = uniforms.find(name);
//	int location;
//	if (loc == uniforms.end()) {
//		location = glGetUniformLocation(program, name);
//		uniforms[name] = location;
//	}
//	else
//		location = loc->second;
//	return location;
//}
//
//void GLShader::setUniform(const char* name, int value) {
//	glUniform1i(getLocation(name), value);
//}
//void GLShader::setUniform(const char* name, float value) {
//	glUniform1f(getLocation(name), value);
//}
//void GLShader::setUniform(const char* name, const glm::vec2& value) {
//	glUniform2f(getLocation(name), value.x, value.y);
//}
//void GLShader::setUniform(const char* name, const glm::vec3& value) {
//	glUniform3f(getLocation(name), value.x, value.y, value.z);
//}
//void GLShader::setUniform(const char* name, const glm::vec4& value) {
//	glUniform4f(getLocation(name), value.x, value.y, value.z, value.w);
//}
//
//void GLShader::setUniform(const char* name, const glm::mat2& value) {
//	glUniformMatrix2fv(getLocation(name), 1, GL_FALSE, &value[0][0]);
//}
//void GLShader::setUniform(const char* name, const glm::mat3& value) {
//	glUniformMatrix3fv(getLocation(name), 1, GL_FALSE, &value[0][0]);
//}
//void GLShader::setUniform(const char* name, const glm::mat4& value) {
//	glUniformMatrix4fv(getLocation(name), 1, GL_FALSE, &value[0][0]);
//}
//
//GLShader::~GLShader() {
//	for (uint shader : shaders)
//		glDeleteShader(shader);
//	glDeleteProgram(program);
//}