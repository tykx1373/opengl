# opengl

一个用于学习 OpenGL 的小型练习项目。手写封装了 VAO / VBO / IBO / Shader / Texture / Renderer / Window，
并用 ImGui 提供测试菜单，运行时可切换不同的测试场景。

## 构建与运行

- 用 Visual Studio 打开 `opengl.slnx`，选择 `Debug | x64` 配置，直接 F5 运行。
- 依赖以静态库方式链接（GLEW `glew32s.lib`、GLFW `glfw3_mt.lib`），运行时不依赖任何外部 DLL。
- 头文件目录与库目录已在 `opengl/opengl.vcxproj` 中配置好，无需额外设置环境变量。

## 目录结构

```
opengl/
├─ src/
│  ├─ Application.cpp      程序入口：主循环、ImGui 菜单、测试项切换
│  ├─ Window.*             窗口与 GL 上下文封装（GLFW）
│  ├─ ImGuiLayer.*         ImGui 初始化与每帧 Begin/End
│  ├─ Renderer.*           清屏与 GL 错误检查（GLCall）
│  ├─ Shader.*             着色器编译/链接与 uniform 设置
│  ├─ Texture.*            纹理加载（stb_image）与绑定
│  ├─ VertexBuffer.* / IndexBuffer.* / VertexArray.*   顶点数据封装
│  ├─ BatchRenderer.*      批渲染：把大量 quad 合并为一次 Draw Call
│  ├─ tests/               各个可切换的测试场景
│  └─ vendor/              以头文件形式引入的第三方库（glm、imgui、stb）
├─ res/
│  ├─ shaders/             GLSL 着色器
│  └─ textures/            贴图资源
└─ vendor/
   ├─ glew/                GLEW 头文件与静态库
   └─ glfw/                GLFW 头文件与静态库
```

## 测试场景

运行后在 ImGui 的 `Test` 面板中选择：

| 测试 | 说明 |
| --- | --- |
| Clear Color | 用滑块调整清屏颜色，验证最基本的渲染循环 |
| 2D Texture | 用两套 MVP 矩阵绘制两张带平移的贴图，验证纹理与 uniform |
| Batch Renderer | 批渲染：上百个方块合并为一次 Draw Call，可在纯色 / 纹理模式间切换 |

## 约定与注意事项

- 着色器变量命名：`a_` 属性、`u_` uniform、`v_` varying。
- ImGui 不要开启 `ViewportsEnable`（多视口会切换 GL 上下文，导致主窗口无法正常呈现），
  也不要开启 `NavEnableGamepad`（在部分 GLFW 版本上会阻塞主循环）。