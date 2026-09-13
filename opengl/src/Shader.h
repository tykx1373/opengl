#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include <glm/fwd.hpp>

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
private:
	std::string m_ShaderPath_Vertex, m_ShaderPath_Fragment;
	unsigned int m_RendererID;
	std::unordered_map <std::string, int> m_UniformLocationCache;
	std::unordered_set<std::string> m_ReportedUniforms; // 已警告过缺失的 uniform，避免重复刷屏
public:
	Shader(const std::string& shaderpath_vertex, const std::string& shaderpath_fragment);
	~Shader();

	void Bind() const;
	void Unbind() const;

	void SetUniform1f(const std::string& name, float value);
	void SetUniform1i(const std::string& name, int value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
private:
	// 分别读取两个独立的 vertex / fragment shader 文件
	static ShaderProgramSource ParseShader(const std::string& shaderpath_vertex, const std::string& shaderpath_fragment);
	static std::string ReadSingleShaderFile(const std::string& path);
	static unsigned int CompileShader(unsigned int type, const std::string& source);
	int CreateShader(const std::string& VertexShader, const std::string& FragmentShader);

	int GetUniformLocation(const std::string& name);
};