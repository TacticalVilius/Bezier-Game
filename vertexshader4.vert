#version 330

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

uniform mat4 model_to_clip_matrix;
uniform vec3 dir_to_light;
uniform vec4 light_intensity;

smooth out vec4 interp_color;

void main()
{
	gl_Position = model_to_clip_matrix * position;

	interp_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}