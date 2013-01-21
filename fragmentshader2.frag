#version 330

smooth in vec4 interp_color;

out vec4 output_color;

void main()
{
	vec4 gamma = vec4(0.454545f);
	gamma.w = 1.0f;
	output_color = pow(interp_color, gamma);
}