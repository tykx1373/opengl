#include "VertexBuffer.h"

#include "Renderer.h"

#include <GL/glew.h>

// 静态 (或外部指定 usage) + 立即填数据
VertexBuffer::VertexBuffer(const void *data, unsigned int size, unsigned int usage)
{
	GLCall(glGenBuffers(1, &m_RendererID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, usage));
}

// 动态：预留 size 字节，先不填数据 (nullptr)，后续用 glBufferSubData 更新
VertexBuffer::VertexBuffer(unsigned int size, unsigned int usage)
{
	GLCall(glGenBuffers(1, &m_RendererID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, nullptr, usage));
}

VertexBuffer::~VertexBuffer()
{
	Unbind();
	GLCall(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
