#pragma once

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Shader.h"

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <vector>

// ---------- 批渲染顶点 ----------
// 所有模式共用同一套顶点结构，才能装进同一个 VBO。
// 字段顺序必须与下面 VAO 的 Push 顺序、以及 shader 里的 layout(location=0..3) 一致。
struct BatchVertex
{
        glm::vec4 position;     // loc 0  位置
        glm::vec4 color;        // loc 1  颜色（纹理模式当 tint）
        glm::vec2 texCoord;     // loc 2  UV（颜色模式填 {0,0}，用不到）
        float     texIndex;     // loc 3  纹理槽位 0~3（颜色模式填 -1，表示不采样）
};

// ---------- BatchRenderer ----------
// 把很多方块攒进一个顶点数组，最后用一次 glDrawArrays 画完（= 1 次 Draw Call）。
class BatchRenderer
{
public:
        // VBO 按 quad 数 × 4 预分配顶点额度；但每个 quad 实际提交 6 个顶点（两个三角形），
        // 所以单批真正能装下的 quad 数 ≈ MAX_VERTICES_PER_BATCH / 6 ≈ 13333，而非命名暗示的 20000。
        static constexpr unsigned int MAX_QUADS_PER_BATCH    = 20000;
        static constexpr unsigned int MAX_VERTICES_PER_BATCH = MAX_QUADS_PER_BATCH * 4;
        static constexpr unsigned int MAX_TEXTURES_PER_BATCH = 4;   // 对应 shader 的 u_Textures[4]

        BatchRenderer();

        // 一次 Begin / End 之间可以提交任意多个 quad，End 时统一画掉
        void Begin(const glm::mat4 &mvp);
        void End();

        // 提交一个纯色方块（颜色模式）
        void SubmitQuadColor(const glm::vec2 &center, const glm::vec2 &half,
                             const glm::vec4 &color);

        // 提交一个贴图方块（纹理模式）。
        // 调用前必须先把纹理绑到 slot 0~3，且传入的 texIndex 要和 slot 编号一致。
        void SubmitQuadTexture(const glm::vec2 &center, const glm::vec2 &half,
                               const glm::vec4 &tint, float texIndex);

        // 立即把已攒的顶点画掉并清空（顶点额度不够时 Submit 内部也会自动调用）
        void Flush();

        // 指定本批使用的 shader（颜色版 / 纹理版）
        void SetShader(Shader *shader) { m_Shader = shader; }

private:
        VertexArray  m_VAO;
        VertexBuffer m_VBO;                     // 动态 VBO：每帧用 glBufferSubData 重写
        Shader      *m_Shader = nullptr;

        std::vector<BatchVertex> m_Vertices;    // 本批已攒的顶点
        bool m_Begun = false;                   // 是否在 Begin / End 之间
};