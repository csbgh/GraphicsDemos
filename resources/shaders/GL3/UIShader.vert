#version 330 core

layout (location = 0) in vec2 v_ui_position;
layout (location = 1) in vec2 v_texCoord;
layout (location = 2) in vec4 v_color;

layout(std140) uniform perFrameUniforms
{
	mat4 viewProjection;
	mat4 uiOrthoProjection;
};

out vec2 texCoord;
out vec4 color;

void main()
{
	texCoord = v_texCoord;
	color = v_color;
	gl_Position = uiOrthoProjection * vec4(v_ui_position, 0, 1);
}