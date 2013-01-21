#ifndef BEZIER_GAME_PLAYER_CAR_KAZAKAUSKAS
#define BEZIER_GAME_PLAYER_CAR_KAZAKAUSKAS

#include "GameObject.h"

class PlayerCar : public GameObject
{
public:
	PlayerCar(BezierGame *bezier_game);
	PlayerCar(BezierGame *bezier_game, std::string name);
	void display(glutil::MatrixStack model_matrix);
	void update();
	void forward();
	void backward();
	void right();
	void left();

	glm::vec3 get_direction();

private:
	void initialize();
	void initialize_vertex_data();
	void initialize_normal_data();
	void initialize_vertex_buffers();
	void initialize_vertex_array_objects();

	// Drawing
	ProgramData player_car_program_data_;
	std::vector<float> player_car_vertex_data_;
	std::vector<float> player_car_normal_data_;
	std::vector<GLshort> player_car_index_data_;
	GLuint normal_buffer_object_;
	GLuint index_buffer_object_;

	// Moving
	glm::vec3 direction_;
	//float speed_;
	glm::vec3 up_direction_;
};

#endif