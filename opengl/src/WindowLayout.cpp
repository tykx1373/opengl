#include "WindowLayout.h"

#include "Renderer.h"
#include "OpebGLProfile.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>

#include <stdexcept>

WindowLayout::WindowLayout(const WindowConfig &cfg)
    : m_cfg(cfg), m_Window(nullptr)
{
    InitGLFW(cfg.glMajor_version, cfg.glMinor_version, cfg.profile);

    glfwSwapInterval(cfg.vsync);

    m_Window = glfwCreateWindow(
        cfg.width, cfg.height, cfg.title,
        cfg.monitor, cfg.share);

    if (!m_Window)
        throw std::runtime_error("Failed to create GLFW window");

    glfwMakeContextCurrent(m_Window);

    // 窗口/帧缓冲尺寸变化时同步更新 OpenGL 视口，避免贴图被拉伸而模糊
    glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow *, int w, int h) {
        glViewport(0, 0, w, h);
    });
    int fbWidth = 0, fbHeight = 0;
    glfwGetFramebufferSize(m_Window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
}

WindowLayout::~WindowLayout()
{
    if (m_Window)
    {
        if (glfwGetCurrentContext() == m_Window)
            glfwMakeContextCurrent(nullptr);
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }
}

WindowLayout::WindowLayout(WindowLayout &&other) noexcept
    : m_Window(other.m_Window)
{
    other.m_Window = nullptr;
}

WindowLayout &WindowLayout::operator=(WindowLayout &&other) noexcept
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

void WindowLayout::Swap() const
{
    glfwSwapBuffers(m_Window);
}

void WindowLayout::MakeCurrent() const
{
    glfwMakeContextCurrent(m_Window);
}

bool WindowLayout::ShouldClose() const
{
    return glfwWindowShouldClose(m_Window);
}

void WindowLayout::PullEvents()
{
    glfwPollEvents();
}

void WindowLayout::InitGLFW(int glMajor_version, int glMinor_version, OPENGL_PROFILE profile)
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