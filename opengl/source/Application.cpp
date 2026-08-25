#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Window.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

int main(void)
{
    Window window;

    // GLEW 初始化（需在 context current 之后）
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "glewInit failed" << std::endl;
        return -1;
    }

    std::cout << "opengl version: " << glGetString(GL_VERSION) << std::endl;

    {
        float positions[] = {
            -0.5f, -0.5f,
             0.5f, -0.5f,
             0.5f,  0.5f,
            -0.5f,  0.5f,
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        VertexBuffer vb(positions, 4 * 2 * sizeof(float));
        VertexArray va;

        VertexBufferLayout layout;
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indices, 6);

        Shader shader("resource/basic.shader");

        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        while (!window.ShouldClose())
        {
            window.PullEvents();
			window.Clear();

            va.Bind();
            ib.Bind();
            shader.Bind();
            shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr););

            window.Swap();
            
        }
    }
    glfwTerminate();
    return 0;
}