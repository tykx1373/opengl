#pragma once

#include <GL/glew.h>    //没用上，但是要glfw在之前调用glew,不然报错
#include <GLFW/glfw3.h>

enum class OPENGL_PROFILE
{
    ANY, CORE, COMPAT
};

struct WindowConfig
{   
   int              width             = 800;
   int              height            = 600;
   const char*      title             = "Untitled"; 
   GLFWmonitor*     monitor           = nullptr;
   GLFWwindow*      share             = nullptr;

   int              glMajor_version   = 4;
   int              glMinor_version   = 6;
   OPENGL_PROFILE   profile           = OPENGL_PROFILE::CORE;

   bool             vsync             = true;
};

class Window
{
private:
    GLFWwindow* m_Window;

public:
    explicit Window(const WindowConfig& config = {});
    ~Window();

    // 禁止拷贝
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    // 允许移动
    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;

    void Swap() const;
    bool ShouldClose() const;    
    static void Clear();
    static void PullEvents();
    
    explicit operator bool() const { return m_Window != nullptr; }

private:
    static void InitGLFW(int major, int minor, OPENGL_PROFILE profile);
    static int GetProfileType(OPENGL_PROFILE profile);
};