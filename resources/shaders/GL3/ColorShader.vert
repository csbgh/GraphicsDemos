#version 330 core

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec4 v_color;

out vec4 color;

void main()
{
	color = v_color;
	gl_Position = vec4(v_position, 1);
}