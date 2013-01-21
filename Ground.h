#ifndef BEZIER_GAME_GROUND_KAZAKAUSKAS
#define BEZIER_GAME_GROUND_KAZAKAUSKAS

#include "GameObject.h"

class Ground : public GameObject
{
public:
	Ground(BezierGame *bezier_game);
	Ground(BezierGame *bezier_game, std::string name);
	void display(glutil::MatrixStack model_matrix);

private:
	void initialize();
	void initialize_vertex_data();
	void initialize_vertex_buffers();
	void initialize_vertex_array_objects();

	ProgramData ground_program_data_;
	std::vector<float> ground_vertex_data_;
};

#endif