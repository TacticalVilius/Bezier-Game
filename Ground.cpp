#include "Ground.h"

#include "BezierGame.h"

Ground::Ground(BezierGame *bezier_game)
	: GameObject(bezier_game)
{
	initialize();
}

Ground::Ground(BezierGame *bezier_game, std::string name)
	: GameObject(bezier_game, name)
{
	initialize();
}

void Ground::initialize()
{
	ground_program_data_ = load_program("vertexshader2.vert", "fragmentshader2.frag");
	initialize_vertex_data();
	initialize_vertex_buffers();
	initialize_vertex_array_objects();

	moving_object_ = false;
	bounding_sphere_check_ = false;
	center_ = glm::vec3(0.0f, 0.0f, 0.0f);
	radius_ = 2.0f;
}

void Ground::display(glutil::MatrixStack model_matrix)
{
	glutil::PushStack push(model_matrix);
	//model_matrix.Translate(position_);
	model_matrix.ApplyMatrix(transformation_.Top());

	glm::mat3 normal_model_to_world_matrix;

	glUseProgram(ground_program_data_.program);
	glBindVertexArray(ground_program_data_.vao);
	glUniformMatrix4fv(ground_program_data_.model_to_clip_mat_unif, 1, GL_FALSE, glm::value_ptr(model_matrix.Top()));
	glUniformMatrix3fv(ground_program_data_.normal_model_to_world_mat_unif, 1, GL_FALSE, glm::value_ptr(normal_model_to_world_matrix));
	glUniform3fv(ground_program_data_.dir_to_light_unif, 1, glm::value_ptr(bezier_game_->get_dir_to_light()));
	glUniform4fv(ground_program_data_.light_intensity_unif, 1, glm::value_ptr(bezier_game_->get_light_intensity()));
	glUniform4fv(ground_program_data_.ambient_intensity_unif, 1, glm::value_ptr(bezier_game_->get_ambient_intensity()));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	glUseProgram(0);
}

void Ground::initialize_vertex_data()
{
	const float ground_data[] = {
		-1.0f, 0.0f, -1.0f,
		1.0f, 0.0f, -1.0f,
		-1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.7f, 0.0f, 1.0f,
		0.0f, 0.7f, 0.0f, 1.0f,
		0.7f, 0.0f, 0.0f, 1.0f,
		0.7f, 0.0f, 0.0f, 1.0f,
	};
	ground_vertex_data_.assign(ground_data, ground_data + sizeof(ground_data)/sizeof(float));
}

void Ground::initialize_vertex_buffers()
{
	glGenBuffers(1, &ground_program_data_.vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, ground_program_data_.vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, ground_vertex_data_.size() * sizeof(float), ground_vertex_data_.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Ground::initialize_vertex_array_objects()
{
	glGenVertexArrays(1, &ground_program_data_.vao);
	glBindVertexArray(ground_program_data_.vao);

	glBindBuffer(GL_ARRAY_BUFFER, ground_program_data_.vertex_buffer_object);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)48);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)96);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}