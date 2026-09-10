#include "Shader.h"

#include "Renderer.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <stdexcept>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

Shader::Shader(const std::string &filepath)
	: m_FilePath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = ParseShader(filepath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
	if (m_RendererID == 0)
		throw std::runtime_error("Failed to create shader program: " + filepath);
	std::cout << "Shader created, new ID=" << m_RendererID << "\n";
}

Shader::~Shader()
{
	Unbind();
	std::cout << "Shader::~Shader() called, ID=" << m_RendererID << "\n";
	GLCall(glDeleteProgram(m_RendererID));	
}

ShaderProgramSource Shader::ParseShader(const std::string &filepath)
{
	std::ifstream stream(filepath);
	if (!stream.is_open())
		throw std::runtime_error("Failed to open shader file: " + filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	std::string ShaderVersion;

	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)//find the shader type declaration line
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}

			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else if (type != ShaderType::NONE)
		{
			if (line.find("#version") != std::string::npos)
			{
				ShaderVersion = line;
			}
			ss[static_cast<int>(type)] << line << '\n';
		}
	}

	std::cout << "Shader Version (Vertex): " << ShaderVersion << "\n";

	if (ss[static_cast<int>(ShaderType::VERTEX)].str().empty() ||
		ss[static_cast<int>(ShaderType::FRAGMENT)].str().empty())
		throw std::runtime_error("Shader file lacks #shader vertex/fragment section: " + filepath);

	return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string &source)
{
	unsigned int id;
	GLCall(id = glCreateShader(type));
	const char *src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));
	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE)
	{
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		std::vector<char> message(length + 1, '\0');
		glGetShaderInfoLog(id, length, &length, message.data());
		std::cout << "Failed to compile " <<
			(type == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< " shader!" << std::endl;
		std::cout << message.data() << std::endl;
		glDeleteShader(id);
		throw std::runtime_error("Shader compilation failed");
	}
	return id;
}

int Shader::CreateShader(const std::string &VertexShader, const std::string &FragmentShader)
{
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, VertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, FragmentShader);
	if (vs == 0 || fs == 0)
	{
		glDeleteShader(vs);
		glDeleteShader(fs);
		return 0;
	}

	unsigned int program;
	GLCall(program = glCreateProgram());
	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));

	GLCall(glLinkProgram(program));
	int linkResult;
	GLCall(glGetProgramiv(program, GL_LINK_STATUS, &linkResult));
	if (linkResult == GL_FALSE)
	{
		int length;
		GLCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length));
		std::vector<char> message(length + 1, '\0');
		glGetProgramInfoLog(program, length, &length, message.data());
		std::cout << "Failed to link shader program!" << std::endl;
		std::cout << message.data() << std::endl;
		glDeleteProgram(program);
		glDeleteShader(vs);
		glDeleteShader(fs);
		return 0;
	}

	GLCall(glValidateProgram(program));
	int validateResult;
	GLCall(glGetProgramiv(program, GL_VALIDATE_STATUS, &validateResult));
	if (validateResult == GL_FALSE)
	{
		int length;
		GLCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length));
		std::vector<char> message(length + 1, '\0');
		glGetProgramInfoLog(program, length, &length, message.data());
		std::cout << "Shader program validation failed (may still work):" << std::endl;
		std::cout << message.data() << std::endl;
	}

	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return program;
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3)
{
 	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

int Shader::GetUniformLocation(const std::string &name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
	{
		if (m_ReportedUniforms.insert(name).second)
			std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
	}
	else
		m_UniformLocationCache[name] = location;
	return location;
}

void Shader::SetUniform1f(const std::string &name, float value)
{
	GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform1i(const std::string &name, int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniformMat4f(const std::string &name, const glm::mat4 &matrix)
{
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}