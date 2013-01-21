#version 330

layout (location = 0) in vec3 position;

uniform vec2 offset;
uniform mat4 perspective_matrix;

void main()
{
	vec4 camera_pos;
	camera_pos.xy = position.xy - 50.0;
	camera_pos.xy /= 50.0;
	camera_pos.z = (-camera_pos.z / 100.0) - 1.0;
	camera_pos.w = 1.0;
	camera_pos += vec4(offset.x, offset.y, 0.0, 0.0);

	gl_Position = perspective_matrix * camera_pos;
}