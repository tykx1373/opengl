#include "ImGuiLayer.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <stdexcept>

ImGuiLayer::ImGuiLayer(GLFWwindow* window, const char* shaderVersion)
	: m_Window(window)
{
	if (shaderVersion != nullptr)
		m_ShaderVersion = shaderVersion; // 默认成员初始化器已给 "#version 330 core"

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsClassic();

	if (!ImGui_ImplGlfw_InitForOpenGL(m_Window, true))
		throw std::runtime_error("Failed to init ImGui GLFW backend");

	if (!ImGui_ImplOpenGL3_Init(m_ShaderVersion))
		throw std::runtime_error("Failed to init ImGui OpenGL3 backend");

	// 高 DPI 屏幕放大 ImGui 字号，否则字体/图标会显得很小
	float contentScaleX = 1.0f, contentScaleY = 1.0f;
	glfwGetWindowContentScale(m_Window, &contentScaleX, &contentScaleY);
	float dpiScale = (contentScaleX > contentScaleY ? contentScaleX : contentScaleY);
	if (dpiScale > 1.0f)
	{
		ImGui::GetStyle().ScaleAllSizes(dpiScale);
		ImGuiIO &io = ImGui::GetIO();
		io.FontGlobalScale = dpiScale;
	}
}

ImGuiLayer::~ImGuiLayer()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiLayer::BeginFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGuiLayer::EndFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO &io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}