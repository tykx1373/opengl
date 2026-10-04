#pragma once

// GLEW 必须先于 GLFW 引入：虽然本头文件未直接使用 GLEW，
// 但借助它的引入来强制任何包含本头文件的编译单元保证顺序。
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "OpebGLProfile.h"

struct WindowConfig
{
    int           width    =   1280;
    int           height   =   720;
    const char*   title    =   "OpenGL Test";
    GLFWmonitor*  monitor  =   nullptr;
    GLFWwindow*   share    =   nullptr;

    int              glMajor_version    = 4;
    int              glMinor_version    = 6;
    OPENGL_PROFILE   profile            = OPENGL_PROFILE::CORE;

    bool             vsync = true;
};

class Window
{
private:
    GLFWwindow* m_Window;
    WindowConfig m_cfg = {};

public:
    explicit Window(const WindowConfig &config = {});
    ~Window();

    // 禁止拷贝
    Window(const Window&) = delete;
    Window &operator=(const Window&) = delete;

    // 允许移动
    Window(Window &&other) noexcept;
    Window &operator=(Window &&other) noexcept;

    GLFWwindow* GetHandle() { return m_Window; }

    void Swap() const;
    void MakeCurrent() const;
    bool ShouldClose() const;
    static void PullEvents();

    explicit operator bool() const { return m_Window != nullptr; }

private:
    static void InitGLFW(int major, int minor, OPENGL_PROFILE profile);
};