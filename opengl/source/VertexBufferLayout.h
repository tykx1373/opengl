#pragma once

#include "Renderer.h"

#include <GL/glew.h>

#include <vector>
#include <stdexcept>
#include <string>

struct VertexBufferElement
{
	unsigned int count;
	unsigned int type;
	unsigned char normalized;

	static unsigned int GetSizeOfType(unsigned int type)
	{
		switch (type)
		{
			case GL_FLOAT:			return sizeof(GLfloat);
			case GL_UNSIGNED_INT:	return sizeof(GLuint);
			case GL_UNSIGNED_BYTE:	return sizeof(GLubyte);
			default:
				throw std::invalid_argument(
					"Unknown vertex attribute type: " + std::to_string(type));
		}
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
public:
	VertexBufferLayout()
		: m_Stride(0) {}

    template<typename T>
    void Push(unsigned int count)
    {
        if constexpr (std::is_same_v<T, float>) {
            m_Elements.push_back({ count, GL_FLOAT, GL_FALSE });
            m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
        }
        else if constexpr (std::is_same_v<T, unsigned int>) {
            m_Elements.push_back({ count, GL_UNSIGNED_INT, GL_FALSE });
            m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
        }
        else if constexpr (std::is_same_v<T, unsigned char>) {
            m_Elements.push_back({ count, GL_UNSIGNED_BYTE, GL_TRUE });
            m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
        }
        else {
            static_assert(sizeof(T) == 0, "Unsupported type for vertex attribute");
        }
    }

	inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }
	inline unsigned int GetStride() const { return m_Stride; }
};
