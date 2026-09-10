#pragma once

#include <GLFW/glfw3.h>

#include <imgui/imgui.h>

class ImGuiLayer
{
private:
	GLFWwindow* m_Window;
	const char *m_ShaderVersion = "#version 330 core";
public:
	explicit ImGuiLayer(GLFWwindow* window, const char* shaderVersion = nullptr);
	~ImGuiLayer();

	ImGuiLayer(const ImGuiLayer&) = delete;
	ImGuiLayer &operator=(const ImGuiLayer&) = delete;

	void BeginFrame();
	void EndFrame();

	void SetcfgFlags(ImGuiConfigFlags flags) const { ImGui::GetIO().ConfigFlags |= flags; }
};