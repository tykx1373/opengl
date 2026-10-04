#pragma once

#include <GL/glew.h>

class VertexBuffer
{
private:
	unsigned int m_RendererID;
public:
	VertexBuffer(const void *data, unsigned int size, unsigned int usage = GL_STATIC_DRAW);
	// 动态 VBO：size = 预留的最大字节数，先不填数据，后续用 glBufferSubData 更新
	VertexBuffer(unsigned int size, unsigned int usage = GL_DYNAMIC_DRAW);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
};