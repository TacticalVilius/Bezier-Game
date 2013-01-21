#include "PlayerCar.h"
#include "BezierGame.h"
#include <iostream>

PlayerCar::PlayerCar(BezierGame *bezier_game)
	: GameObject(bezier_game)
{
	initialize();
}

PlayerCar::PlayerCar(BezierGame *bezier_game, std::string name)
	: GameObject(bezier_game, name)
{
	player_car_program_data_ = load_program("vertexshader2.vert", "fragmentshader2.frag");
	initialize();
}

void PlayerCar::initialize()
{
	initialize_vertex_data();
	initialize_normal_data();
	initialize_vertex_buffers();
	initialize_vertex_array_objects();

	direction_ = glm::vec3(0.0f, 0.0f, 1.0f);
	//speed_ = 0.0f;
	up_direction_ = glm::vec3(0.0f, 1.0f, 0.0f);

	center_ = glm::vec3(3.0f, 1.5f, 6.0f);
	radius_ = 6.0f;
	moving_object_ = true;
	bounding_sphere_check_ = true;
	bsp_root_ = build_bsp_tree();
}

void PlayerCar::display(glutil::MatrixStack model_matrix)
{
	glutil::PushStack push(model_matrix);
	
	//model_matrix.Translate(position_);
	//model_matrix.ApplyMatrix(rotation_.Top());
	//model_matrix.Scale(scale_);
	model_matrix.ApplyMatrix(transformation_.Top());

	glm::mat3 normal_model_to_world_matrix(rotation_.Top());

	glUseProgram(player_car_program_data_.program);
	glBindVertexArray(player_car_program_data_.vao);
	glUniformMatrix4fv(player_car_program_data_.model_to_clip_mat_unif, 1, GL_FALSE, glm::value_ptr(model_matrix.Top()));
	glUniformMatrix3fv(player_car_program_data_.normal_model_to_world_mat_unif, 1, GL_FALSE, glm::value_ptr(normal_model_to_world_matrix));
	glUniform3fv(player_car_program_data_.dir_to_light_unif, 1, glm::value_ptr(bezier_game_->get_dir_to_light()));
	glUniform4fv(player_car_program_data_.light_intensity_unif, 1, glm::value_ptr(bezier_game_->get_light_intensity()));
	glUniform4fv(player_car_program_data_.ambient_intensity_unif, 1, glm::value_ptr(bezier_game_->get_ambient_intensity()));
	
	glDrawElements(GL_TRIANGLES, player_car_index_data_.size(), GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
}

void PlayerCar::update()
{
	//move(direction_ * speed_);
	move(velocity_);
	GameObject::update();
}

void PlayerCar::forward()
{
	//speed_ = 0.001f;
	update_velocity(direction_ * 0.001f);
}

void PlayerCar::backward()
{
	//speed_ = 0.0f;
	update_velocity(-direction_ * 0.001f);
}

void PlayerCar::right()
{
	//if (speed_ == 0.0f) return;
	glm::vec3 prev_direction = direction_;
	direction_ += (0.001f * 10 * glm::normalize(glm::cross(up_direction_, direction_)));
	direction_ = glm::normalize(direction_);
	rotation_.Rotate(up_direction_, acosf(glm::dot(prev_direction, direction_)) * 360.0f / (2.0f*3.1415f));
	set_velocity(glm::normalize(get_velocity() + direction_ * 0.001f) * glm::length(get_velocity()));
}

void PlayerCar::left()
{
	//if (speed_ == 0.0f) return;
	glm::vec3 prev_direction = direction_;
	direction_ += (0.001f * 10 * glm::normalize(glm::cross(direction_, up_direction_)));
	direction_ = glm::normalize(direction_);
	rotation_.Rotate(up_direction_, -1 * acosf(glm::dot(prev_direction, direction_)) * 360.0f / (2.0f*3.1415f));
	set_velocity(glm::normalize(get_velocity() + direction_ * 0.001f) * glm::length(get_velocity()));
}

void PlayerCar::initialize_normal_data()
{
	const float normal_data[] = {
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,

		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.0f, 1.0f/sqrtf(2.0f), -1.0f/sqrtf(2.0f),
		0.0f, 1.0f/sqrtf(2.0f), -1.0f/sqrtf(2.0f),
		0.0f, 1.0f/sqrtf(2.0f), -1.0f/sqrtf(2.0f),
		0.0f, 1.0f/sqrtf(2.0f), -1.0f/sqrtf(2.0f),

		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
	};
	player_car_normal_data_.assign(normal_data, normal_data + sizeof(normal_data)/sizeof(float));
	for (unsigned int i = 0; i < player_car_normal_data_.size(); ++i) {
		player_car_vertex_data_.push_back(player_car_normal_data_[i]);
	}
}

void PlayerCar::initialize_vertex_data()
{
	polygons_.push_back(new PolygonFace(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(6.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 12.0f)));
	polygons_.push_back(new PolygonFace(glm::vec3(6.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 12.0f), glm::vec3(6.0f, 0.0f, 12.0f)));
	polygons_.push_back(new PolygonFace(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(6.0f, 3.0f, 0.0f), glm::vec3(0.0f, 3.0f, 12.0f)));
	polygons_.push_back(new PolygonFace(glm::vec3(6.0f, 3.0f, 0.0f), glm::vec3(0.0f, 3.0f, 12.0f), glm::vec3(6.0f, 3.0f, 12.0f)));
	polygons_.push_back(new PolygonFace(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 12.0f), glm::vec3(0.0f, 3.0f, 12.0f)));
	polygons_.push_back(new PolygonFace(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f, 3.0f, 12.0f)));
	polygons_.push_back(new PolygonFace(glm::vec3(6.0f, 0.0f, 0.0f), glm::vec3(6.0f, 0.0f, 12.0f), glm::vec3(6.0f, 3.0f, 12.0f)));
	polygons_.push_back(new PolygonFace(glm::vec3(6.0f, 0.0f, 0.0f), glm::vec3(6.0f, 3.0f, 0.0f), glm::vec3(6.0f, 3.0f, 12.0f)));
	polygons_.push_back(new PolygonFace(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(6.0f, 0.0f, 0.0f), glm::vec3(6.0f, 3.0f, 0.0f)));
	polygons_.push_back(new PolygonFace(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(6.0f, 3.0f, 0.0f)));
	polygons_.push_back(new PolygonFace(glm::vec3(0.0f, 0.0f, 12.0f), glm::vec3(6.0f, 0.0f, 12.0f), glm::vec3(6.0f, 3.0f, 12.0f)));
	polygons_.push_back(new PolygonFace(glm::vec3(0.0f, 0.0f, 12.0f), glm::vec3(0.0f, 3.0f, 12.0f), glm::vec3(6.0f, 3.0f, 12.0f)));

	const float vertex_data[] = {
		0.0f, 0.0f, 0.10f,	//0
		0.0f, 2.0f, 0.0f,	//1
		6.0f, 2.0f, 0.0f,	//2
		6.0f, 0.0f, 0.0f,	//3

		0.0f, 2.0f, 0.0f,	//4
		0.0f, 2.0f, 4.0f,	//5
		6.0f, 2.0f, 4.0f,	//6
		6.0f, 2.0f, 0.0f,	//7

		0.0f, 2.0f, 4.0f,	//8
		0.0f, 3.0f, 5.0f,	//9
		6.0f, 3.0f, 5.0f,	//10
		6.0f, 2.0f, 4.0f,	//11

		0.0f, 3.0f, 5.0f,	//12
		0.0f, 3.0f, 10.0f,	//13
		6.0f, 3.0f, 10.0f,	//14
		6.0f, 3.0f, 5.0f,	//15
		
		6.0f, 0.0f, 0.0f,	//16
		6.0f, 2.0f, 0.0f,	//17
		6.0f, 2.0f, 12.0f,	//18
		6.0f, 0.0f, 12.0f,	//19
		
		6.0f, 2.0f, 4.0f,	//20
		6.0f, 3.0f, 5.0f,	//21
		6.0f, 2.0f, 5.0f,	//22
		
		6.0f, 2.0f, 10.0f,	//23
		6.0f, 3.0f, 10.0f,	//24

		0.0f, 0.0f, 0.0f,	//25
		0.0f, 0.0f, 12.0f,	//26
		6.0f, 0.0f, 12.0f,	//27
		6.0f, 0.0f, 0.0f,	//28

		0.0f, 0.0f, 12.0f,	//29
		0.0f, 2.0f, 12.0f,	//30
		6.0f, 2.0f, 12.0f,	//31
		6.0f, 0.0f, 12.0f,	//32

		0.0f, 2.0f, 10.0f,	//33
		0.0f, 2.0f, 12.0f,	//34
		6.0f, 2.0f, 12.0f,	//35
		6.0f, 2.0f, 10.0f,	//36

		0.0f, 2.0f, 10.0f,	//37
		0.0f, 3.0f, 10.0f,	//38
		6.0f, 3.0f, 10.0f,	//39
		6.0f, 2.0f, 10.0f,	//40

		0.1f, 0.1f, 0.1f,	//41
		0.0f, 0.0f, 12.0f,	//42
		0.0f, 2.0f, 12.0f,	//43
		0.0f, 2.0f, 0.0f,	//44

		0.0f, 2.0f, 5.0f,	//45
		0.0f, 3.0f, 5.0f,	//46
		0.0f, 2.0f, 4.0f,	//47

		0.0f, 3.0f, 10.0f,	//48
		0.0f, 2.0f, 10.0f,	//49
		
		0.3f, 0.0f, 0.0f, 1.0f,
		0.3f, 0.0f, 0.0f, 1.0f,
		0.3f, 0.0f, 0.0f, 1.0f,
		0.3f, 0.0f, 0.0f, 1.0f,

		0.3f, 0.0f, 0.0f, 1.0f,
		0.3f, 0.0f, 0.0f, 1.0f,
		0.3f, 0.0f, 0.0f, 1.0f,
		0.3f, 0.0f, 0.0f, 1.0f,

		0.01f, 0.01f, 0.01f, 1.0f,
		0.01f, 0.01f, 0.01f, 1.0f,
		0.01f, 0.01f, 0.01f, 1.0f,
		0.01f, 0.01f, 0.01f, 1.0f,

		0.3f, 0.0f, 0.0f, 1.0f,
		0.3f, 0.0f, 0.0f, 1.0f,
		0.3f, 0.0f, 0.0f, 1.0f,
		0.3f, 0.0f, 0.0f, 1.0f,
		
		0.0f, 0.0f, 0.6f, 1.0f,
		0.0f, 0.0f, 0.6f, 1.0f,
		0.0f, 0.0f, 0.6f, 1.0f,
		0.0f, 0.0f, 0.6f, 1.0f,
		
		0.01f, 0.01f, 0.01f, 1.0f,
		0.01f, 0.01f, 0.01f, 1.0f,
		0.01f, 0.01f, 0.01f, 1.0f,
		
		0.0f, 0.0f, 0.6f, 1.0f,
		0.0f, 0.0f, 0.6f, 1.0f,
		
		0.3f, 0.0f, 0.0f, 1.0f,
		0.3f, 0.0f, 0.0f, 1.0f,
		0.3f, 0.0f, 0.0f, 1.0f,
		0.3f, 0.0f, 0.0f, 1.0f,
		
		0.3f, 0.0f, 0.0f, 1.0f,
		0.3f, 0.0f, 0.0f, 1.0f,
		0.3f, 0.0f, 0.0f, 1.0f,
		0.3f, 0.0f, 0.0f, 1.0f,
		
		0.3f, 0.0f, 0.0f, 1.0f,
		0.3f, 0.0f, 0.0f, 1.0f,
		0.3f, 0.0f, 0.0f, 1.0f,
		0.3f, 0.0f, 0.0f, 1.0f,
		
		0.01f, 0.01f, 0.01f, 1.0f,
		0.01f, 0.01f, 0.01f, 1.0f,
		0.01f, 0.01f, 0.01f, 1.0f,
		0.01f, 0.01f, 0.01f, 1.0f,
		
		0.0f, 0.0f, 0.6f, 1.0f,
		0.0f, 0.0f, 0.6f, 1.0f,
		0.0f, 0.0f, 0.6f, 1.0f,
		0.0f, 0.0f, 0.6f, 1.0f,
		
		0.01f, 0.01f, 0.01f, 1.0f,
		0.01f, 0.01f, 0.01f, 1.0f,
		0.01f, 0.01f, 0.01f, 1.0f,
		
		0.0f, 0.0f, 0.6f, 1.0f,
		0.0f, 0.0f, 0.6f, 1.0f,
	};
	player_car_vertex_data_.assign(vertex_data, vertex_data + sizeof(vertex_data)/sizeof(float));

	const GLshort index_data[] = {
		0, 1, 2,
		0, 3, 2,

		4, 5, 6,
		4, 7, 6,

		8, 9, 10,
		8, 11, 10,

		12, 13, 14,
		12, 15, 14,
		
		16, 17, 18,
		16, 19, 18,
		
		20, 21, 22,
		
		21, 22, 23,
		21, 24, 23,

		25, 26, 27,
		25, 28, 27,

		29, 30, 31,
		29, 32, 31,

		33, 34, 35,
		33, 36, 35,

		37, 38, 39,
		37, 40, 39,

		41, 42, 43,
		41, 44, 43,

		45, 46, 47,

		46, 47, 48,
		46, 49, 48,
	};
	player_car_index_data_.assign(index_data, index_data + sizeof(index_data)/sizeof(GLshort));
}

void PlayerCar::initialize_vertex_buffers()
{
	glGenBuffers(1, &player_car_program_data_.vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, player_car_program_data_.vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, player_car_vertex_data_.size() * sizeof(float), player_car_vertex_data_.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/*
	glGenBuffers(1, &normal_buffer_object_);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_object_);
	glBufferData(GL_ARRAY_BUFFER, player_car_normal_data_.size() * sizeof(float), player_car_normal_data_.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	*/
	glGenBuffers(1, &index_buffer_object_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, player_car_index_data_.size() * sizeof(GLshort), player_car_index_data_.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void PlayerCar::initialize_vertex_array_objects()
{
	glGenVertexArrays(1, &player_car_program_data_.vao);
	glBindVertexArray(player_car_program_data_.vao);

	//size_t color_data_offset = sizeof(float) * player_car_vertex_data_.size() / 2;
	size_t color_data_offset = 50 * 3 * sizeof(float);
	size_t normal_data_offset = color_data_offset + 50 * 4 * sizeof(float);

	glBindBuffer(GL_ARRAY_BUFFER, player_car_program_data_.vertex_buffer_object);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)normal_data_offset);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void *)color_data_offset);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object_);
	/*
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_object_);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	*/
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

glm::vec3 PlayerCar::get_direction()
{
	return direction_;
}