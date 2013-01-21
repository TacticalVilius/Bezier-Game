#ifndef BEZIER_GAME_GAME_OBJECT_KAZAKAUSKAS
#define BEZIER_GAME_GAME_OBJECT_KAZAKAUSKAS

#include <string>
#include <vector>

#include "Includes.h"
#include "BSP_Node.h"
#include "PolygonFace.h"

class BezierGame;

struct ProgramData
{
	GLuint program;
	GLuint model_to_clip_mat_unif;
	GLuint normal_model_to_world_mat_unif;
	GLuint dir_to_light_unif;
	GLuint light_intensity_unif;
	GLuint ambient_intensity_unif;
	GLuint vertex_buffer_object;
	GLuint vao;
};

class GameObject
{
public:
	GameObject(BezierGame *bezier_game);
	GameObject(BezierGame *bezier_game, std::string name);
	~GameObject();
	virtual void display(glutil::MatrixStack model_matrix) = 0;
	virtual void do_time_step();
	virtual void undo_time_step();

	virtual void move(glm::vec3 offset);
	virtual void rotate(glm::mat4 rotation);
	virtual void scale(float factor);

	bool moving_object();
	bool bounding_sphere_check();
	virtual glm::vec3 get_center();
	virtual float get_radius();
	std::string get_name();
	glm::vec3 get_position();
	PolygonFace* get_original_polygon(int index) const;
	void get_polygon(int index, PolygonFace &polygon) const;
	int get_polygon_count() const;
	BSP_Node* get_bsp_root_node() const;
	glm::vec3 get_velocity() const;
	void set_velocity(const glm::vec3 &velocity);
	void update_velocity(const glm::vec3 &vector);
	float get_mass() const;
	void set_mass(float mass);

	int bsp_polygon_count_;
	
protected:
	GLuint load_shader(GLenum shader_type, const std::string &shader_file_name);
	GLuint create_program(std::vector<GLuint> shader_list);
	ProgramData load_program(const std::string &vertex_shader_file_name, const std::string &fragment_shader_file_name);
	BSP_Node* build_bsp_tree();
	void make_bsp_node(BSP_Node *node, std::vector<PolygonFace *> polygons);
	int check_vector_against_plane(BSP_Node *plane, glm::vec3 vector);
	void split_polygon_with_plane(const BSP_Node &plane, const PolygonFace &polygon, PolygonFace &front_polygon, PolygonFace &back_polygon);
	void update_polygon(PolygonFace &polygon) const;
	void update_transformation_matrix();

	BezierGame *bezier_game_;
	std::string name_;
	bool moving_object_;
	bool bounding_sphere_check_;

	std::vector<PolygonFace *> polygons_;
	glm::vec3 center_;
	float radius_;
	BSP_Node *bsp_root_;

	glm::vec3 position_;
	glutil::MatrixStack rotation_;
	float scale_;
	glutil::MatrixStack transformation_;
	glm::vec3 velocity_;
	float mass_;
};

#endif