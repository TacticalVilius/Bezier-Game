#include "Track.h"

#include "Bezier.h"
#include "BezierGame.h"

Track::Track(BezierGame *bezier_game, std::vector<float> base_points, int bezier_points_amount, float track_width, float track_fatness)
	: GameObject(bezier_game),
	  base_points_(base_points),
	  bezier_points_amount_(bezier_points_amount),
	  track_width_(track_width),
	  track_fatness_(track_fatness)
{
	initialize();
}

Track::Track(BezierGame *bezier_game, float base_points[], int elements_count, int bezier_points_amount, float track_width, float track_fatness)
	: GameObject(bezier_game),
	  bezier_points_amount_(bezier_points_amount),
	  track_width_(track_width),
	  track_fatness_(track_fatness)
{
	base_points_.assign(base_points, base_points + elements_count);
	initialize();
}

Track::Track(BezierGame *bezier_game, std::string name, std::vector<float> base_points, int bezier_points_amount, float track_width, float track_fatness)
	: GameObject(bezier_game, name),
	  base_points_(base_points),
	  bezier_points_amount_(bezier_points_amount),
	  track_width_(track_width),
	  track_fatness_(track_fatness)
{
	initialize();
}

Track::Track(BezierGame *bezier_game, std::string name, float base_points[], int elements_count, int bezier_points_amount, float track_width, float track_fatness)
	: GameObject(bezier_game, name),
	  bezier_points_amount_(bezier_points_amount),
	  track_width_(track_width),
	  track_fatness_(track_fatness)
{
	base_points_.assign(base_points, base_points + elements_count);
	initialize();
}

void Track::initialize()
{
	top_track_program_data_ = load_program("vertexshader.vert", "fragmentshader2.frag");
	bottom_track_program_data_ = load_program("vertexshader.vert", "fragmentshader2.frag");
	std::vector<float> top_track_vertex_data(bezier_points_amount_ * 12);
	std::vector<float> bottom_track_vertex_data(bezier_points_amount_ * 12);
	initialize_vertex_data(top_track_vertex_data, bottom_track_vertex_data);
	initialize_vertex_buffers(top_track_vertex_data, bottom_track_vertex_data);
	initialize_vertex_array_objects();
	bsp_root_ = build_bsp_tree();

	moving_object_ = false;
	bounding_sphere_check_ = false;
	center_ = glm::vec3(0.0f, 0.0f, 0.0f);
	radius_ = 1000.0f;
}

void Track::display(glutil::MatrixStack model_matrix)
{
	//glutil::PushStack push(model_matrix);
	//model_matrix.do_stuff(...);
	glutil::PushStack push(model_matrix);
	//model_matrix.Translate(position_);
	model_matrix.ApplyMatrix(transformation_.Top());

	glm::mat3 normal_model_to_world_matrix;

	glUseProgram(top_track_program_data_.program);
	glBindVertexArray(top_track_program_data_.vao);
	glUniformMatrix4fv(top_track_program_data_.model_to_clip_mat_unif, 1, GL_FALSE, glm::value_ptr(model_matrix.Top()));
	glUniformMatrix3fv(top_track_program_data_.normal_model_to_world_mat_unif, 1, GL_FALSE, glm::value_ptr(normal_model_to_world_matrix));
	glUniform3fv(top_track_program_data_.dir_to_light_unif, 1, glm::value_ptr(bezier_game_->get_dir_to_light()));
	glUniform4fv(top_track_program_data_.light_intensity_unif, 1, glm::value_ptr(bezier_game_->get_light_intensity()));
	glUniform4fv(top_track_program_data_.ambient_intensity_unif, 1, glm::value_ptr(bezier_game_->get_ambient_intensity()));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, bezier_points_amount_ * 2);

	glBindVertexArray(0);
	glUseProgram(0);

	glUseProgram(bottom_track_program_data_.program);
	glBindVertexArray(bottom_track_program_data_.vao);
	glUniformMatrix4fv(bottom_track_program_data_.model_to_clip_mat_unif, 1, GL_FALSE, glm::value_ptr(model_matrix.Top()));
	glUniformMatrix3fv(bottom_track_program_data_.normal_model_to_world_mat_unif, 1, GL_FALSE, glm::value_ptr(normal_model_to_world_matrix));
	glUniform3fv(bottom_track_program_data_.dir_to_light_unif, 1, glm::value_ptr(bezier_game_->get_dir_to_light()));
	glUniform4fv(bottom_track_program_data_.light_intensity_unif, 1, glm::value_ptr(bezier_game_->get_light_intensity()));
	glUniform4fv(bottom_track_program_data_.ambient_intensity_unif, 1, glm::value_ptr(bezier_game_->get_ambient_intensity()));

	glDrawArrays(GL_TRIANGLE_STRIP, bezier_points_amount_ * 2, bezier_points_amount_ * 2);

	glBindVertexArray(0);
	glUseProgram(0);
}

void Track::create_top_track(std::vector<float> &top_track_data)
{
	std::vector<Point3d *> base;
	unsigned int base_points_amount = base_points_.size();
	for (unsigned int i = 0; i < base_points_amount; i += 3) {
		Point3d *point = new Point3d();
		point->x = base_points_[i];
		point->y = base_points_[i+1];
		point->z = base_points_[i+2];
		base.push_back(point);
	}
	BezierCurve curve(base);
	curve.calculateBezier(bezier_points_amount_);
	for (int i = 0; i < bezier_points_amount_; ++i) {
		top_track_data[i*6] = curve.getBezierPoint(i)->x;
		top_track_data[i*6+1] = curve.getBezierPoint(i)->y;
		top_track_data[i*6+2] = curve.getBezierPoint(i)->z;
	}
	glm::vec3 plane_vec = glm::vec3(0.0f, 1.0f, 0.0f);
	for (int i = 1; i < bezier_points_amount_ - 1; ++i) {
		glm::vec3 second_vec;
		second_vec.x = curve.getBezierPoint(i+1)->x - curve.getBezierPoint(i-1)->x;
		second_vec.y = curve.getBezierPoint(i+1)->y - curve.getBezierPoint(i-1)->y;
		second_vec.z = curve.getBezierPoint(i+1)->z - curve.getBezierPoint(i-1)->z;
		glm::vec3 track_dir = glm::normalize(glm::cross(plane_vec, second_vec)) * track_width_;

		top_track_data[i*6+3] = top_track_data[i*6] + track_dir.x;
		top_track_data[i*6+4] = top_track_data[i*6+1] + track_dir.y;
		top_track_data[i*6+5] = top_track_data[i*6+2] + track_dir.z;
	}
	top_track_data[3] = top_track_data[0] + track_width_;
	top_track_data[4] = top_track_data[1];
	top_track_data[5] = top_track_data[2];
	top_track_data[bezier_points_amount_*6-3] = top_track_data[bezier_points_amount_*6-6] + track_width_;
	top_track_data[bezier_points_amount_*6-2] = top_track_data[bezier_points_amount_*6-5];
	top_track_data[bezier_points_amount_*6-1] = top_track_data[bezier_points_amount_*6-4];

	for (int i = 1; i < bezier_points_amount_ - 1; ++i) {
		glm::vec3 first_vec;
		first_vec.x = top_track_data[i*6+6] - top_track_data[i*6-6];
		first_vec.y = top_track_data[i*6+7] - top_track_data[i*6-5];
		first_vec.z = top_track_data[i*6+8] - top_track_data[i*6-4];
		glm::vec3 second_vec;
		second_vec.x = top_track_data[i*6+3] - top_track_data[i*6];
		second_vec.y = top_track_data[i*6+4] - top_track_data[i*6+1];
		second_vec.z = top_track_data[i*6+5] - top_track_data[i*6+2];
		glm::vec3 norm_cross = glm::normalize(glm::cross(first_vec, second_vec));

		top_track_data[(bezier_points_amount_ + i) * 6] = norm_cross.x;
		top_track_data[(bezier_points_amount_ + i) * 6 + 1] = norm_cross.y;
		top_track_data[(bezier_points_amount_ + i) * 6 + 2] = norm_cross.z;
		top_track_data[(bezier_points_amount_ + i) * 6 + 3] = norm_cross.x;
		top_track_data[(bezier_points_amount_ + i) * 6 + 4] = norm_cross.y;
		top_track_data[(bezier_points_amount_ + i) * 6 + 5] = norm_cross.z;
	}
	top_track_data[bezier_points_amount_ * 6] = top_track_data[bezier_points_amount_ * 6 + 6];
	top_track_data[bezier_points_amount_ * 6 + 1] = top_track_data[bezier_points_amount_ * 6 + 7];
	top_track_data[bezier_points_amount_ * 6 + 2] = top_track_data[bezier_points_amount_ * 6 + 8];
	top_track_data[bezier_points_amount_ * 6 + 3] = top_track_data[bezier_points_amount_ * 6 + 9];
	top_track_data[bezier_points_amount_ * 6 + 4] = top_track_data[bezier_points_amount_ * 6 + 10];
	top_track_data[bezier_points_amount_ * 6 + 5] = top_track_data[bezier_points_amount_ * 6 + 11];
	
	top_track_data[bezier_points_amount_ * 12 - 6] = top_track_data[bezier_points_amount_ * 12 - 12];
	top_track_data[bezier_points_amount_ * 12 - 5] = top_track_data[bezier_points_amount_ * 12 - 11];
	top_track_data[bezier_points_amount_ * 12 - 4] = top_track_data[bezier_points_amount_ * 12 - 10];
	top_track_data[bezier_points_amount_ * 12 - 3] = top_track_data[bezier_points_amount_ * 12 - 9];
	top_track_data[bezier_points_amount_ * 12 - 2] = top_track_data[bezier_points_amount_ * 12 - 8];
	top_track_data[bezier_points_amount_ * 12 - 1] = top_track_data[bezier_points_amount_ * 12 - 7];

	for (unsigned int i = 0; i < top_track_data.size() / 2; i = i + 3) {
		glm::vec3 first = glm::vec3(top_track_data[i], top_track_data[i + 1], top_track_data[i + 2]);
		glm::vec3 second = glm::vec3(top_track_data[i + 3], top_track_data[i + 4], top_track_data[i + 5]);
		glm::vec3 third = glm::vec3(top_track_data[i + 6], top_track_data[i + 7], top_track_data[i + 8]);
		polygons_.push_back(new PolygonFace(first, second, third));
	}
}

void Track::create_bottom_track(const std::vector<float> &top_track_vertex_data, std::vector<float> &bottom_track_data)
{
	for (int i = 0; i < bezier_points_amount_ * 2; ++i) {
		bottom_track_data[i * 3] = top_track_vertex_data[i * 3] + (top_track_vertex_data[bezier_points_amount_ * 6 + i * 3] * -1 * track_fatness_);
		bottom_track_data[i * 3 + 1] = top_track_vertex_data[i * 3 + 1] + (top_track_vertex_data[bezier_points_amount_ * 6 + i * 3 + 1] * -1 * track_fatness_);
		bottom_track_data[i * 3 + 2] = top_track_vertex_data[i * 3 + 2] + (top_track_vertex_data[bezier_points_amount_ * 6 + i * 3 + 2] * -1 * track_fatness_);
	}
	for (int i = bezier_points_amount_ * 6; i < bezier_points_amount_ * 12; ++i) {
		bottom_track_data[i] = top_track_vertex_data[i] * -1;
	}
	for (unsigned int i = 0; i < bottom_track_data.size() / 2; i = i + 3) {
		glm::vec3 first = glm::vec3(bottom_track_data[i], bottom_track_data[i + 1], bottom_track_data[i + 2]);
		glm::vec3 second = glm::vec3(bottom_track_data[i + 3], bottom_track_data[i + 4], bottom_track_data[i + 5]);
		glm::vec3 third = glm::vec3(bottom_track_data[i + 6], bottom_track_data[i + 7], bottom_track_data[i + 8]);
		polygons_.push_back(new PolygonFace(first, second, third));
	}
}

void Track::initialize_vertex_data(std::vector<float> &top_track_vertex_data, std::vector<float> &bottom_track_vertex_data)
{
	create_top_track(top_track_vertex_data);
	create_bottom_track(top_track_vertex_data, bottom_track_vertex_data);
}

void Track::initialize_vertex_buffers(const std::vector<float> &top_track_vertex_data, const std::vector<float> &bottom_track_vertex_data)
{
	glGenBuffers(1, &top_track_program_data_.vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, top_track_program_data_.vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, top_track_vertex_data.size() * sizeof(float), top_track_vertex_data.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &bottom_track_program_data_.vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, bottom_track_program_data_.vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, bottom_track_vertex_data.size() * sizeof(float), bottom_track_vertex_data.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Track::initialize_vertex_array_objects()
{
	glGenVertexArrays(1, &top_track_program_data_.vao);
	glBindVertexArray(top_track_program_data_.vao);

	glBindBuffer(GL_ARRAY_BUFFER, top_track_program_data_.vertex_buffer_object);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(bezier_points_amount_ * 24));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	glGenVertexArrays(1, &bottom_track_program_data_.vao);
	glBindVertexArray(bottom_track_program_data_.vao);

	glBindBuffer(GL_ARRAY_BUFFER, bottom_track_program_data_.vertex_buffer_object);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(bezier_points_amount_ * 24));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}