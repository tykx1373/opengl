#include "IndexBuffer.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "WindowLayout.h"
#include "ImGuiLayer.h"

#include "tests/TestClearColor.h"
#include "tests/Test.h"
#include "tests/TestTexture2D.h"

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <iostream>

int main(void)
{
	WindowLayout window;

	if (glewInit() != GLEW_OK)
	{
		std::cerr << "glewInit failed" << std::endl;
		return -1;
	}
	std::cout << "opengl version: " << glGetString(GL_VERSION) << std::endl;

	ImGuiLayer gui(window.GetHandle());

	ImGuiConfigFlags flags = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad | 
		ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

	gui.SetcfgFlags(flags);
	
	test::Test *currentTest = nullptr;
	test::TestMenu *testMenu = new test::TestMenu(currentTest);
	currentTest = testMenu;
		
	testMenu->RegisterTest<test::TestClearColor>("Clear Color");
	testMenu->RegisterTest<test::TestTexture2D>("2D Texture");

	while (!window.ShouldClose())
	{
		window.MakeCurrent();   // 副窗口渲染后切走的 context，在这里恢复为主窗口
		window.PullEvents();

		// 窗口最小化时跳过
		if (glfwGetWindowAttrib(window.GetHandle(), GLFW_ICONIFIED))
		{
			ImGui_ImplGlfw_Sleep(10);
			continue;
		}

		// 窗口不可见时也跳过
		if (!glfwGetWindowAttrib(window.GetHandle(), GLFW_VISIBLE))
		{
			ImGui_ImplGlfw_Sleep(10);
			continue;
		}

		Renderer::Clear();       // 先清屏
		currentTest->OnRender(); // 再渲染场景

		gui.BeginFrame();
		if (currentTest)
		{
			ImGui::Begin("Test");
			currentTest->OnImGuiRender();
			currentTest->OnUpdate(0.0f);
			if (currentTest != testMenu && ImGui::Button("<-"))
			{
				delete currentTest;
				currentTest = testMenu;
			}
			ImGui::End();
		}

		gui.EndFrame();
		window.Swap();
	}

	delete currentTest;
	test::ResetSharedResources();   // 在 glfwTerminate() 前释放共享 Shader/Texture，避免 context 销毁后 glDelete* 卡死
	return 0;
}