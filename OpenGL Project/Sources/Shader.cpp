#include "../Headers/Shader.h"
#include "../Headers/ErrorHandler.h"
#include <fstream>
#include <string>
#include<GL/glew.h>
#include "../Headers/Variables.h"


using namespace std;

Shader::Shader(const std::string& path)
	:pth(path)
{
	ShaderSrc src = GetShaderSource(path);
	_id = CreateShader(src.vertex, src.fragment);
	glm::mat4 projection = glm::ortho(0.0f, (float)s_width, (float)s_height, 0.0f, -1.0f, 1.0f);
	SetMatrix4("projection", projection);
}

Shader::~Shader() {
	std::cout << "shader dtor" << std::endl;
	UnBind();
}

ShaderSrc Shader::GetShaderSource(const string& path) {
	ifstream stream(path);
	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	string line;
	string shaders[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line)) {
		if (line == "#shader vertex") {
			type = ShaderType::VERTEX;
		}
		if (line == "#shader fragment") {
			type = ShaderType::FRAGMENT;
		}

		if (type == ShaderType::VERTEX && line != "#shader vertex") {
			shaders[0] += line + "\n";
		}
		if (type == ShaderType::FRAGMENT && line != "#shader fragment") {
			shaders[1] += line + "\n";
		}
	}

	return { shaders[0], shaders[1] };
}

void Shader::Bind() const
{
 	lcall(glUseProgram(_id));
}

void Shader::UnBind() const
{
	lcall(glUseProgram(0));
}

unsigned int Shader::CompileShader(unsigned int type, const string& source) {

	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	//ERROR HANDLER
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (!result) {

		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << endl;
		cout << message << endl;
	}
	return id;
}

unsigned int Shader::CreateShader(const string& vertexShader, const string& fragmentShader) {

	unsigned int program = glCreateProgram();

	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;

}

void Shader::SetMatrix4(const char* name, const glm::mat4 value) const{
	Bind();
	auto location = glGetUniformLocation(_id, name);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetFloat4(const char* name, float a, float b, float c, float d) const{
	Bind();
	auto location = glGetUniformLocation(_id, name);
	lcall(glUniform4f(location, a, b, c, d));
}
void Shader::SetFloat2(const char* name, float a, float b) const{
	Bind();
	auto location = glGetUniformLocation(_id, name);
	lcall(glUniform2f(location, a, b));
}
void Shader::SetFloat(const char* name, float a) const{
	Bind();
	auto location = glGetUniformLocation(_id, name);
	lcall(glUniform1f(location, a));
}

void Shader::SetInt(const char* name, int a) const {
	Bind();
	auto location = glGetUniformLocation(_id, name);
	lcall(glUniform1i(location, a));
}





