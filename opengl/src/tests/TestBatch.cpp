#include "TestBatch.h"

#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"

#include <GL/glew.h>

#include <imgui/imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <random>
#include <cmath>
#include <vector>

namespace test
{
	TestBatch::TestBatch()
		: m_Proj(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f)),
		  m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)))
	{
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		// 共用一个 vertex shader + 两套 fragment，各自独立 Shader 实例
		m_BatchColorShader = std::make_unique<Shader>(
			"res/shaders/batch_vertex.shader",
			"res/shaders/batch_color_fragment.shader");
		m_BatchTextureShader = std::make_unique<Shader>(
			"res/shaders/batch_vertex.shader",
			"res/shaders/batch_texture_fragment.shader");

		// 加载纹理资源
		m_TextureA = std::make_unique<Texture>("res/textures/Taffy.png");
		m_TextureB = std::make_unique<Texture>("res/textures/texture.png");

		// 给纹理 shader 告诉它采样器映射 slot 0/1/2/3
		m_BatchTextureShader->Bind();
		int slots[4] = { 0, 1, 2, 3 };
		m_BatchTextureShader->SetUniform1iv("u_Textures", 4, slots);

		// 初始化 BatchRenderer
		m_Batcher.SetShader(m_BatchColorShader.get());
	}

	TestBatch::~TestBatch()
	{
		// 析构前先清干净所有纹理单元，防止 Texture::Unbind 只解绑当前激活单元的 bug
		// 导致 slot 0 上残留已标记删除的纹理，影响后续 TestTexture2D
		Texture::UnbindAllSlots(2);
	}

	void TestBatch::OnUpdate(float deltaTime)
	{
	}

	void TestBatch::OnRender()
	{
		GLCall(glClearColor(0.05f, 0.07f, 0.10f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		glm::mat4 mvp = m_Proj * m_View;

		switch (m_Mode)
		{
		case BatchMode::Color:
		{
			m_Batcher.SetShader(m_BatchColorShader.get());
			m_Batcher.Begin(mvp);

			// 按网格生成若干方块，不同颜色 
			// 开方后向上取整
			int perSide = static_cast<int>(std::ceil(std::sqrt(static_cast<float>(m_QuadCount)))); 
			float step  = 2.0f / perSide; // 投影空间 [-1,1] 总长 2

			std::mt19937 rng(1234u); // 固定随机种子，保证每帧颜色一致
			std::uniform_real_distribution<float> colorDist(0.2f, 1.0f); // 颜色随机范围 [0.2,1.0]，避免太暗

			int placed = 0;
			for (int y = 0; y < perSide && placed < m_QuadCount; y++)
			{
				for (int x = 0; x < perSide && placed< m_QuadCount; x++)
				{
					float cx = -1.0f + step * (x + 0.5f);
					float cy = -1.0f + step * (y + 0.5f);

					glm::vec4 c(colorDist(rng), colorDist(rng), colorDist(rng), 0.85f); // 随机颜色 + 半透明
					m_Batcher.SubmitQuadColor({ cx, cy }, { step * 0.5f - m_TileSpacing, step * 0.5f - m_TileSpacing }, c);
					placed++;
				}
			}

			m_Batcher.End();
			break;
		}

		case BatchMode::Texture:
		{
			m_Batcher.SetShader(m_BatchTextureShader.get());

			// 把纹理先绑定好 (shader 的 uniform sampler2D 已设置好映射)
			m_TextureA->Bind(0);
			m_TextureB->Bind(1);

			m_Batcher.Begin(mvp);

			int perSide = static_cast<int>(std::ceil(std::sqrt(static_cast<float>(m_QuadCount))));
			float step  = 2.0f / perSide;

			for (int y = 0; y < perSide; y++)
			{
				for (int x = 0; x < perSide; x++)
				{
					float cx = -1.0f + step * (x + 0.5f);
					float cy = -1.0f + step * (y + 0.5f);

					// 两张纹理轮着用
					float texIdx = ((x + y) & 1) ? 1.0f : 0.0f;

					// 颜色调制：用不同的 tint，让效果明显
					glm::vec4 tint = texIdx == 0.0f
						? glm::vec4(0.9f, 0.9f, 1.0f, 1.0f)
						: glm::vec4(1.0f, 0.9f, 0.9f, 1.0f);

					m_Batcher.SubmitQuadTexture({ cx, cy }, { step * 0.5f - m_TileSpacing, step * 0.5f - m_TileSpacing },
					                            tint, texIdx);
				}
			}

			m_Batcher.End();
			break;
		}
		}
	}

	void TestBatch::OnImGuiRender()
	{
		static const char* modeNames[] = { "Color", "Texture" };
		int current = static_cast<int>(m_Mode);
		if (ImGui::Combo("Batch Mode", &current, modeNames, IM_ARRAYSIZE(modeNames)))
			m_Mode = static_cast<BatchMode>(current);

		ImGui::SliderInt("Quad Count", &m_QuadCount,   1, 2000);
		ImGui::SliderFloat("Spacing", &m_TileSpacing, 0.0f, 0.02f);

		ImGui::Separator();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
			1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}

