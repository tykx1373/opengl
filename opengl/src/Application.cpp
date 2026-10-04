#include "IndexBuffer.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Window.h"
#include "ImGuiLayer.h"

#include "tests/TestClearColor.h"
#include "tests/Test.h"
#include "tests/TestTexture2D.h"
#include "tests/TestBatch.h"

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <iostream>

int main(void)
{
	Window window;

	if (glewInit() != GLEW_OK)
	{
		std::cerr << "glewInit failed" << std::endl;
		return -1;
	}
	std::cout << "opengl version: " << glGetString(GL_VERSION) << std::endl;

	ImGuiLayer gui(window.GetHandle());

	// 注意 1：不要开启 ImGuiConfigFlags_ViewportsEnable，多视口会切换 GL 上下文导致主窗口无法正常呈现。
	// 注意 2：不要开启 ImGuiConfigFlags_NavEnableGamepad，该标志会让 ImGui 每帧调用
	//         glfwGetGamepadState()，在本机 GLFW 上会阻塞，导致主循环卡死、窗口内容不再刷新（花屏/卡住）。
	ImGuiConfigFlags flags = ImGuiConfigFlags_NavEnableKeyboard |
		ImGuiConfigFlags_DockingEnable;

	gui.SetcfgFlags(flags);

	test::Test *currentTest = nullptr;
	test::TestMenu *testMenu = new test::TestMenu(currentTest);
	currentTest = testMenu;

	testMenu->RegisterTest<test::TestClearColor>("Clear Color");
	testMenu->RegisterTest<test::TestTexture2D>("2D Texture");
	testMenu->RegisterTest<test::TestBatch>("Batch Renderer");

	// 启动后停留在菜单，由用户选择测试项
	currentTest = testMenu;

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

		// ImGui 渲染后会残留 FBO / glViewport / GL_SCISSOR_TEST 状态，
		// 每帧开始前必须恢复到主窗口的默认帧缓冲、完整视口并关闭裁剪，
		// 否则 OnRender 的 glClear / 绘制会落到错误目标或被裁剪，出现花屏/白屏。
		int fbW = 0, fbH = 0;
		glfwGetFramebufferSize(window.GetHandle(), &fbW, &fbH);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, fbW, fbH);
		glDisable(GL_SCISSOR_TEST);

		Renderer::Clear();       // 先清屏
		currentTest->OnRender(); // 再渲染场景

		gui.BeginFrame();
		if (currentTest)
		{
			ImGui::Begin("Test");
			currentTest->OnImGuiRender();
			currentTest->OnUpdate(0.0f);
			if (currentTest != testMenu && ImGui::Button("Exit"))
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