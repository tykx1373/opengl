#include "Renderer.h"

#include <GL/glew.h>

#include <iostream>

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

const char* GLErrorString(GLenum error)
{
	switch (error)
	{
		case GL_INVALID_ENUM:                  return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE:                 return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION:             return "GL_INVALID_OPERATION";
		case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
		case GL_OUT_OF_MEMORY:                 return "GL_OUT_OF_MEMORY";
		case GL_STACK_UNDERFLOW:               return "GL_STACK_UNDERFLOW";
		case GL_STACK_OVERFLOW:                return "GL_STACK_OVERFLOW";
		default:                               return "UNKNOWN_ERROR";
	}
}

bool GLLogCall(const char *function, const char *file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << GLErrorString(error) << "): " << function <<
			" " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}

void Renderer::Draw(const VertexArray &va, const IndexBuffer &ib, const Shader &shader) const
{
	va.Bind();
	ib.Bind();
	shader.Bind();

	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Clear()
{
	GLCall(glClear(GL_COLOR_BUFFER_BIT));
}
