#ifndef BEZIER_GAME_TRACK_KAZAKAUSKAS
#define BEZIER_GAME_TRACK_KAZAKAUSKAS

#include "GameObject.h"

class Track : public GameObject
{
public:
	Track(BezierGame *bezier_game, std::vector<float> base_points, int bezier_points_amount, float track_width, float track_fatness);
	Track(BezierGame *bezier_game, float base_points[], int elements_count, int bezier_points_amount, float track_width, float track_fatness);
	Track(BezierGame *bezier_game, std::string name, std::vector<float> base_points, int bezier_points_amount, float track_width, float track_fatness);
	Track(BezierGame *bezier_game, std::string name, float base_points[], int elements_count, int bezier_points_amount, float track_width, float track_fatness);
	void display(glutil::MatrixStack model_matrix);

private:
	void initialize();
	void initialize_vertex_data(std::vector<float> &top_track_vertex_data, std::vector<float> &bottom_track_vertex_data);
	void initialize_vertex_buffers(const std::vector<float> &top_track_vertex_data, const std::vector<float> &bottom_track_vertex_data);
	void create_top_track(std::vector<float> &top_track_data);
	void create_bottom_track(const std::vector<float> &top_track_vertex_data, std::vector<float> &bottom_track_data);
	void initialize_vertex_array_objects();

	std::vector<float> base_points_;
	const int bezier_points_amount_;
	ProgramData top_track_program_data_;
	ProgramData bottom_track_program_data_;
	const float track_width_;
	const float track_fatness_;
};

#endif