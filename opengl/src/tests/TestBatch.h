#pragma once

#include "Test.h"

#include "../BatchRenderer.h"

#include <glm/glm.hpp>

#include <memory>
#include <string>

class Shader;
class Texture;

namespace test
{
	enum class BatchMode
	{
		Color,   // 纯颜色绘制
		Texture  // 纹理 + 颜色调制
	};

	class TestBatch : public Test
	{
	public:
		TestBatch();
		~TestBatch() override;

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;

	private:
		// 两套 shader 实例，共用 batch_vertex
		std::unique_ptr<Shader>  m_BatchColorShader;       // 颜色模式
		std::unique_ptr<Shader>  m_BatchTextureShader;     // 纹理模式

		std::unique_ptr<Texture> m_TextureA;   // slot 0
		std::unique_ptr<Texture> m_TextureB;   // slot 1

		BatchRenderer m_Batcher;

		glm::mat4 m_Proj;   // 正交投影
		glm::mat4 m_View;   // 单位矩阵即可 (摄像机不动)

		BatchMode m_Mode = BatchMode::Texture;

		int   m_QuadCount    = 500;       // 本帧画多少个方块
		float m_TileSpacing  = 0.003f;    // 方块之间间距
	};
}

