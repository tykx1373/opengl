#version 330 core

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoord;

out vec2 v_TexCoord;

uniform mat4 u_MVP;// Modle View Projection

void main()
{
	gl_Position = u_MVP * a_position;
	v_TexCoord = a_texCoord;
};