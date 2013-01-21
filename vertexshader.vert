#version 330

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

uniform mat4 model_to_clip_matrix;
uniform mat3 normal_model_to_world_matrix;
uniform vec3 dir_to_light;
uniform vec4 light_intensity;
uniform vec4 ambient_intensity;

smooth out vec4 interp_color;

void main()
{
	gl_Position = model_to_clip_matrix * position;

	float cos_ang_incidence = dot(normalize(normal_model_to_world_matrix * normal), dir_to_light);
	cos_ang_incidence = clamp(cos_ang_incidence, 0, 1);
	interp_color = (light_intensity * vec4(0.0f, 0.0f, 1.0f, 1.0f) * cos_ang_incidence) + (vec4(0.0f, 0.0f, 1.0f, 1.0f) * ambient_intensity);
}