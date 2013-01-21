#version 330

layout (location = 0) in vec4 position;

uniform vec2 offset;
uniform mat4 perspective_matrix;

void main()
{
	vec4 camera_pos = position + vec4(offset.x, offset.y, 0.0, 0.0);
	gl_Position = perspective_matrix * camera_pos;
}