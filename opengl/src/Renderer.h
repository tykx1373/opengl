#pragma once

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

#include <GL/glew.h>

#define GLCall(x) do { \
    GLClearError(); \
    x; \
    ASSERT(GLLogCall(#x, __FILE__, __LINE__)); \
} while(0)

#ifdef _DEBUG
	#define ASSERT(x) if (!(x)) __debugbreak();
#else
	#define ASSERT(x) ((void)0)
#endif


const char* GLErrorString(GLenum error);
void GLClearError();
bool GLLogCall(const char *function, const char *file, int line);

class Renderer
{
public:
	void Draw(const VertexArray &va, const IndexBuffer &ib, const Shader &shader) const;
	static void Clear();
};