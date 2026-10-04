#include "Texture.h"

#include "Renderer.h"

#include <GL/glew.h>

#include <stdexcept>
#include <iostream>

# define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Texture::Texture(const std::string &path)
	: m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr),
	m_Height(0), m_Width(0), m_BPP(0)
{
	GLCall(glGenTextures(1, &m_RendererID));
	std::cout << "Texture created, new ID=" << m_RendererID << "\n";
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	stbi_set_flip_vertically_on_load(1);
	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);
	if (!m_LocalBuffer)
	{
		throw std::runtime_error("Failed to load texture: " + std::string(path) +
			". Reason: " + stbi_failure_reason());
	}

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	// 上传前显式重置像素存储状态：其他库（如 ImGui 后端）可能在 GL_UNPACK_ROW_LENGTH /
	// GL_UNPACK_ALIGNMENT 上留下脏状态。若 GL_UNPACK_ROW_LENGTH 非 0，glTexImage2D 会按
	// 该值与源数据宽度不一致的行跨距读取像素，导致纹理被横向"拉丝"/错行（看上去就是糊的）。
	// 这里临时改为紧致 RGBA 布局（行跨距=宽度，4 字节对齐），上传后恢复原值。
	GLint prevRowLength = 0, prevAlignment = 4;
	GLCall(glGetIntegerv(GL_UNPACK_ROW_LENGTH, &prevRowLength));
	GLCall(glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevAlignment));
	GLCall(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));
	GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
	// 生成 mipmap：批量渲染时方块在屏幕上往往只有几十像素，
	// 纹理被大幅缩小。若只有 GL_LINEAR（无 mipmap），缩小采样会严重走样，
	// 表现为高频摩尔纹/横向条纹（看起来就是"花"）。
	GLCall(glGenerateMipmap(GL_TEXTURE_2D));

	// 恢复调用前的像素存储状态，避免把我们的设置泄漏给其他调用方
	GLCall(glPixelStorei(GL_UNPACK_ROW_LENGTH, prevRowLength));
	GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, prevAlignment));

	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	if (m_LocalBuffer)
		stbi_image_free(m_LocalBuffer);
}

Texture::~Texture()
{
	Unbind();
	std::cout << "Texture::~Texture() called, ID=" << m_RendererID << "\n";
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture::UnbindAllSlots(unsigned int maxSlots)
{
	for (unsigned int i = 0; i < maxSlots; i++)
	{
		GLCall(glActiveTexture(GL_TEXTURE0 + i));
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	}
	GLCall(glActiveTexture(GL_TEXTURE0));
}
