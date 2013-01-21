#ifndef BEZIER_GAME_KAZAKAUSKAS
#define BEZIER_GAME_KAZAKAUSKAS

#include <vector>

#include "Includes.h"

#include "GameObject.h"
#include "PlayerCar.h"
#include "Track.h"

class BezierGame
{
public:
	BezierGame(int argc, char **argv);
	~BezierGame();
	void start();
	void display();
	void reshape(int width, int height);
	void keyboard(unsigned char key, int x, int y);
	static void print_matrix(glm::mat4 matrix);
	static void print_matrix(glm::mat3 matrix);
	static void print_vector(glm::vec4 vector);
	static void print_vector(glm::vec3 vector);

	glm::vec3 get_dir_to_light();
	glm::vec4 get_light_intensity();
	glm::vec4 get_ambient_intensity();
	void test();

private:
	void init();
	void initialize_game_objects();
	float deg_to_rad(float deg_ang);
	glm::vec3 resolve_camera_position();
	glm::mat4 calc_world_to_camera_matrix(const glm::vec3 &camera_position, const glm::vec3 &camera_target, const glm::vec3 &up_vector);
	void add_game_object(GameObject *game_object);
	void check_collision();
	bool check_bounding_spheres(GameObject *first_object, GameObject *second_object);
	bool check_collision_bsp(GameObject *first_object, GameObject *second_object);
	bool objects_intersect_bsp(BSP_Node *bsp_node, GameObject *object);
	int check_plane_versus_bounding_sphere(BSP_Node *bsp_node, glm::vec3 center, float radius);
	bool polygons_intersect(PolygonFace *first_polygon, PolygonFace *second_polygon);
	bool axis_separates_polygons(glm::vec3 &separating_direction, PolygonFace *first_polygon, PolygonFace *second_polygon);
	bool polygon_lies_in_plane(PolygonFace *polygon, float a, float b, float c, float d);
	void resolve_collision_moving(GameObject *first_object, GameObject *second_object);

	std::vector<GameObject *> moving_game_objects_;
	std::vector<GameObject *> non_moving_game_objects_;
	glm::mat4 camera_to_clip_matrix_;
	const float z_near_;
	const float z_far_;
	glm::vec3 dir_to_light_;
	glm::vec4 light_intensity_;
	glm::vec4 ambient_intensity_;
	glm::vec3 cam_rel_pos_sphere_;
	glm::vec3 camera_target_;
	glm::vec3 up_vector_;

	Track *track_;
	PlayerCar *player_car_;
};

#endif