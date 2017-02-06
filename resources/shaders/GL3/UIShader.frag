#version 330 core

in vec2 texCoord;
in vec4 color;

uniform sampler2D tex;

out vec4 out_color;

void main()
{
	out_color = texture(tex, texCoord) * color;
}