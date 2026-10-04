#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;

uniform sampler2D u_Textures[4]; // 最多同时绑定 4 张纹理

void main()
{
        int idx = int(v_TexIndex);
        vec4 texSample;

        if (idx == 0)       texSample = texture(u_Textures[0], v_TexCoord);
        else if (idx == 1)  texSample = texture(u_Textures[1], v_TexCoord);
        else if (idx == 2)  texSample = texture(u_Textures[2], v_TexCoord);
        else if (idx == 3)  texSample = texture(u_Textures[3], v_TexCoord);
        else                texSample = vec4(1.0); // idx>=4 或 idx<0 时走纯色

        color = texSample * v_Color;
};
