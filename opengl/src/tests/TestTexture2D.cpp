#include "TestTexture2D.h"

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"

#include <GL/glew.h>

#include <imgui/imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace test
{
	namespace
	{
		std::shared_ptr<Shader>  g_Shader;
		std::shared_ptr<Texture> g_Texture;
	}

	void ResetSharedResources()
	{
		g_Shader.reset();
		g_Texture.reset();
	}

	TestTexture2D::TestTexture2D()
		: m_Proj(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f)),
		m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
		m_TranslationA(0, 0, 0), m_TranslationB(1, 1, 0)
	{
		float positions[] = 
		{
			-0.3f, -0.4f, 0.0f, 0.0f,
			 0.3f, -0.4f, 1.0f, 0.0f,
			 0.3f,  0.4f, 1.0f, 1.0f,
			-0.3f,  0.4f, 0.0f, 1.0f
		};

		unsigned int indices[] = 
		{
			0, 1, 2,
			2, 3, 0
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		if (!g_Shader)
			g_Shader = std::make_shared<Shader>("res/shaders/vertex.shader", "res/shaders/fragment.shader");
		if (!g_Texture)
			g_Texture = std::make_shared<Texture>("res/textures/Taffy.png");

		m_Shader = g_Shader;
		m_Texture = g_Texture;

		m_VAO = std::make_unique<VertexArray>();
		m_VAO->Bind();

		m_VBO = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));
		m_VBO->Bind();

		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		m_VAO->AddBuffer(*m_VBO, layout);
		m_IBO = std::make_unique<IndexBuffer>(indices, 6);

		m_Shader->Bind();
		m_Shader->SetUniform1i("u_Texture", 0);
	}
	
	TestTexture2D::~TestTexture2D()
	{
		m_Shader.reset();
		m_Texture.reset();
	}

	void TestTexture2D::OnUpdate(float deltaTime)
	{

	}

	void TestTexture2D::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		Renderer render;

		m_Texture->Bind();
		m_Shader->Bind();
		m_Shader->SetUniform1i("u_Texture", 0);

		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
			glm::mat4 mvp = m_Proj * m_View * model;
			m_Shader->Bind();
			m_Shader->SetUniformMat4f("u_MVP", mvp);
			render.Draw(*m_VAO, *m_IBO, *m_Shader);
		}

		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);
			glm::mat4 mvp = m_Proj * m_View * model;
			m_Shader->Bind();
			m_Shader->SetUniformMat4f("u_MVP", mvp);
			render.Draw(*m_VAO, *m_IBO, *m_Shader);
		}
	}

	void TestTexture2D::OnImGuiRender()
	{
		ImGui::SliderFloat3("TranslationA", &(m_TranslationA.x), -1.0f, 1.0f);
		ImGui::SliderFloat3("TranslationB", &(m_TranslationB.x), -1.0f, 1.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}