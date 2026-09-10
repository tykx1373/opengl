#pragma once

#include "Test.h"

#include <glm/glm.hpp>

#include <memory>

class VertexArray;
class VertexBuffer;
class Shader;
class Texture;
class IndexBuffer;

namespace test
{
	class TestTexture2D : public Test
	{
	public:
		TestTexture2D();
		~TestTexture2D();
		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		std::shared_ptr<Shader> m_Shader;   
		std::shared_ptr<Texture> m_Texture; 
		std::unique_ptr<VertexArray> m_VAO; 
		std::unique_ptr<VertexBuffer> m_VBO;
		std::unique_ptr<IndexBuffer> m_IBO;

		glm::mat4 m_Proj, m_View;
		glm::vec3 m_TranslationA, m_TranslationB;
	};

	// 释放 TestTexture2D 共享的 Shader/Texture（必须在 glfwTerminate() 之前调用）
	static void ResetSharedResources();
}