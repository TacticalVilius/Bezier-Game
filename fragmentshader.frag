#version 330

out vec4 output_color;

void main()
{
	vec4 gamma(0.454545f);
	gamma.w = 1.0f;
	output_color = pow(vec4(0.0f, 0.0f, 0.7f, 1.0f), gamma);
}