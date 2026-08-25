#include "Window.h"

#include "Renderer.h"

#include <GL/glew.h>

#include <stdexcept>

Window::Window(const WindowConfig& cfg)
    : m_Window(nullptr)
{
    InitGLFW(cfg.glMajor_version, cfg.glMinor_version, cfg.profile);

    glfwSwapInterval(cfg.vsync);

    m_Window = glfwCreateWindow(
        cfg.width, cfg.height, cfg.title,
        cfg.monitor, cfg.share);

    if (!m_Window)
        throw std::runtime_error("Failed to create GLFW m_Window");

    glfwMakeContextCurrent(m_Window);
}

Window::~Window()
{
    if (m_Window)
        if (glfwGetCurrentContext() == m_Window)
            glfwMakeContextCurrent(nullptr);
        glfwDestroyWindow(m_Window);
}

Window::Window(Window&& other) noexcept
    : m_Window(other.m_Window)
{
    other.m_Window = nullptr;
}

Window& Window::operator=(Window&& other) noexcept
{
    if (this != &other)
    {
        if (m_Window)
            glfwDestroyWindow(m_Window);
        m_Window = other.m_Window;
        other.m_Window = nullptr;
    }
    return *this;
}

void Window::Swap() const
{
    glfwSwapBuffers(m_Window);
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(m_Window);
}

void Window::Clear()
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT););
}

void Window::PullEvents()
{
    GLCall(glfwPollEvents(););
}

void Window::InitGLFW(int glMajor_version, int glMinor_version, OPENGL_PROFILE profile)
{
    [[maybe_unused]] static bool glfw_initialized = []() {
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize GLFW");
        return true;
        }();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glMajor_version);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glMinor_version);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GetProfileType(profile));
}

int Window::GetProfileType(OPENGL_PROFILE profile)
{
    switch (profile)
    {
        case OPENGL_PROFILE::ANY:   return GLFW_OPENGL_ANY_PROFILE;
        case OPENGL_PROFILE::CORE:  return GLFW_OPENGL_CORE_PROFILE;
        case OPENGL_PROFILE::COMPAT: return GLFW_OPENGL_COMPAT_PROFILE;
        default:
            throw std::runtime_error("Unknown OpenGL profile");
    }
}