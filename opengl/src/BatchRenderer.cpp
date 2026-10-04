#include "BatchRenderer.h"

#include "Renderer.h"   // GLCall

#include <stdexcept>

// 一个 quad：4 个角 → 6 个顶点（两个三角形，不用 IndexBuffer）
//   三角形 1: bl → br → tr
//   三角形 2: tr → tl → bl
// texIndex = -1 表示纯色（片元 shader 不采样纹理）
static void PushQuad(std::vector<BatchVertex> &verts,
                     const glm::vec2 &center, const glm::vec2 &half,
                     const glm::vec4 &color, float texIndex)
{
        float x0 = center.x - half.x;
        float y0 = center.y - half.y;
        float x1 = center.x + half.x;
        float y1 = center.y + half.y;

        BatchVertex bl, br, tr, tl;

        bl.position = glm::vec4(x0, y0, 0.0f, 1.0f);
        br.position = glm::vec4(x1, y0, 0.0f, 1.0f);
        tr.position = glm::vec4(x1, y1, 0.0f, 1.0f);
        tl.position = glm::vec4(x0, y1, 0.0f, 1.0f);

        bl.texCoord = glm::vec2(0.0f, 0.0f);
        br.texCoord = glm::vec2(1.0f, 0.0f);
        tr.texCoord = glm::vec2(1.0f, 1.0f);
        tl.texCoord = glm::vec2(0.0f, 1.0f);

        bl.color = br.color = tr.color = tl.color = color;
        bl.texIndex = br.texIndex = tr.texIndex = tl.texIndex = texIndex;

        verts.push_back(bl); verts.push_back(br); verts.push_back(tr);
        verts.push_back(tr); verts.push_back(tl); verts.push_back(bl);
}

BatchRenderer::BatchRenderer()
        : m_VBO(MAX_VERTICES_PER_BATCH * sizeof(BatchVertex), GL_DYNAMIC_DRAW)
{
        // 预分配顶点缓冲，避免每帧反复 reallocate
        m_Vertices.reserve(MAX_VERTICES_PER_BATCH);

        m_VAO.Bind();
        VertexBufferLayout layout;
        layout.Push<float>(4);  // position → loc 0
        layout.Push<float>(4);  // color    → loc 1
        layout.Push<float>(2);  // texCoord → loc 2
        layout.Push<float>(1);  // texIndex → loc 3
        m_VAO.AddBuffer(m_VBO, layout);   // Push 顺序必须与 BatchVertex 字段顺序一致
}

void BatchRenderer::Begin(const glm::mat4 &mvp)
{
        if (!m_Shader)
                throw std::runtime_error("BatchRenderer::Begin() 前必须先 SetShader()");

        m_Vertices.clear();
        m_Begun = true;

        m_Shader->Bind();
        m_Shader->SetUniformMat4f("u_MVP", mvp);
}

void BatchRenderer::End()
{
        Flush();          // 最后一批统一画掉
        m_Begun = false;
}

void BatchRenderer::SubmitQuadColor(const glm::vec2 &center, const glm::vec2 &half,
                                    const glm::vec4 &color)
{
        if (!m_Begun)
                throw std::runtime_error("SubmitQuadColor 必须在 Begin/End 之间调用");

        if (m_Vertices.size() + 6 > MAX_VERTICES_PER_BATCH)
                Flush();   // 装不下就先画掉一批，腾出空间

        PushQuad(m_Vertices, center, half, color, -1.0f);
}

void BatchRenderer::SubmitQuadTexture(const glm::vec2 &center, const glm::vec2 &half,
                                      const glm::vec4 &tint, float texIndex)
{
        if (!m_Begun)
                throw std::runtime_error("SubmitQuadTexture 必须在 Begin/End 之间调用");

        if (texIndex < 0.0f || texIndex >= static_cast<float>(MAX_TEXTURES_PER_BATCH))
                throw std::runtime_error("SubmitQuadTexture: texIndex 必须在 0~3");

        if (m_Vertices.size() + 6 > MAX_VERTICES_PER_BATCH)
                Flush();

        PushQuad(m_Vertices, center, half, tint, texIndex);
}

void BatchRenderer::Flush()
{
        if (m_Vertices.empty())
                return;

        m_VAO.Bind();
        m_VBO.Bind();

        // 把 CPU 端攒好的一整块顶点灌进动态 VBO
        GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0,
                               m_Vertices.size() * sizeof(BatchVertex),
                               m_Vertices.data()));

        // 一次画完本批所有顶点
        GLCall(glDrawArrays(GL_TRIANGLES, 0,
                            static_cast<GLsizei>(m_Vertices.size())));

        m_Vertices.clear();
}