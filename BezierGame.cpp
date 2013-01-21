#include "BezierGame.h"

#include <iostream>
#include <limits>

#include "Ground.h"

BezierGame *bezier_game;

void g_display()
{
	if (bezier_game)
		bezier_game->display();
}

void g_reshape(int width, int height)
{
	if (bezier_game)
		bezier_game->reshape(width, height);
}

void g_keyboard(unsigned char key, int x, int y)
{
	if (bezier_game)
		bezier_game->keyboard(key, x, y);
}

BezierGame::BezierGame(int argc, char **argv)
	: z_near_(0.01f),
	  z_far_(100.0f),
	  dir_to_light_(glm::vec3(-0.57735, 0.57735, -0.57735)),
	  light_intensity_(glm::vec4(0.8f, 0.8f, 0.8f, 1.0f)),
	  ambient_intensity_(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)),
	  cam_rel_pos_sphere_(0.5f, 90.0f, 150.0f),
	  camera_target_(0.0f, 0.0f, 0.0f),
	  up_vector_(0.0f, 1.0f, 0.0f)
{
	glutInit(&argc, argv);

	int width = 500;
	int height = 500;

	GLuint display_mode = GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH | GLUT_STENCIL;
	glutInitDisplayMode(display_mode);

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	int window = glutCreateWindow(argv[0]);

	glload::LoadFunctions();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	if (!glload::IsVersionGEQ(3, 3)) {
		std::cerr << "Running OpenGL version is " << glload::GetMajorVersion() << "." << glload::GetMinorVersion() << ". Version 3.3 or higher is needed." << std::endl;
		glutDestroyWindow(0);
		throw 0;
	}

	init();
	glutDisplayFunc(g_display);
	glutReshapeFunc(g_reshape);
	glutKeyboardFunc(g_keyboard);
}

BezierGame::~BezierGame()
{
	for (unsigned int i = 0; i < moving_game_objects_.size(); ++i) {
		delete moving_game_objects_[i];
		moving_game_objects_[i] = 0;
	}
	for (unsigned int i = 0; i < non_moving_game_objects_.size(); ++i) {
		delete non_moving_game_objects_[i];
		non_moving_game_objects_[i] = 0;
	}
}

void BezierGame::test()
{
	glm::vec3 vertex1(0.0f, 0.0f, 0.0f);
	glm::vec3 vertex2(1.0f, 0.0f, 0.0f);
	glm::vec3 vertex3(0.5f, 1.0f, 0.0f);
	PolygonFace polygon1(vertex1, vertex2, vertex3);

	glm::vec3 vertex4(0.5f, 0.0f, -0.5f);
	glm::vec3 vertex5(1.0f, 1.0f, 0.5f);
	glm::vec3 vertex6(1.5f, 0.0f, 1.0f);
	PolygonFace polygon2(vertex4, vertex5, vertex6);

	glm::vec3 direction(1.0f, 0.0f, -1.0f);
	/*
	if (axis_separates_polygons(direction, &polygon1, &polygon2)) {
		std::cout << "separates" << std::endl;
	}
	else {
		std::cout << "does not separate" << std::endl;
	}
	*/
	polygons_intersect(&polygon1, &polygon2);
}

void BezierGame::start()
{
	//test();
	glutMainLoop();
}

void BezierGame::add_game_object(GameObject *game_object)
{
	if (game_object->moving_object())
		moving_game_objects_.push_back(game_object);
	else
		non_moving_game_objects_.push_back(game_object);
}

void BezierGame::initialize_game_objects()
{
	/*
	float base_points[] = {
		-1.0f, 0.0f, -1.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.5f, 0.0f,
		0.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
	};*/
	float base_points[] = {
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
	};
	const int bezier_points_amount = 100;
	const float track_width = 0.1f;
	const float track_fatness = 0.01f;
	track_ = new Track(this, "Track", base_points, sizeof(base_points)/sizeof(float), bezier_points_amount, track_width, track_fatness);
	track_->move(glm::vec3(0.0f, 0.01f, 1.1f));
	add_game_object(track_);

	GameObject *ground = new Ground(this, "Ground");
	ground->move(glm::vec3(0.0f, 0.0f, 1.1f));
	add_game_object(ground);

	player_car_ = new PlayerCar(this, "Player Car");
	player_car_->scale(1.0f/120.0f);
	glutil::MatrixStack rotation;
	rotation.RotateY(180.0f);
	player_car_->rotate(rotation.Top());
	player_car_->move(glm::vec3(0.0f, 0.01f, 0.1f));
	add_game_object(player_car_);

	PlayerCar *ai_car = new PlayerCar(this, "AI Car");
	ai_car->move(glm::vec3(-0.3f, 0.01f, 1.0f));
	ai_car->scale(1.0f/120.0f);
	ai_car->rotate(rotation.Top());
	add_game_object(ai_car);
}

void BezierGame::init()
{
	initialize_game_objects();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
}

float BezierGame::deg_to_rad(float deg_ang)
{
	return deg_ang * 0.017453f;
}

glm::vec3 BezierGame::resolve_camera_position()
{
	/*
	float theta = deg_to_rad(cam_rel_pos_sphere_.y);
	float phi = deg_to_rad(cam_rel_pos_sphere_.z);
	float sin_theta = sinf(theta);
	float cos_theta = cosf(theta);
	float sin_phi = sinf(phi);
	float cos_phi = cosf(phi);

	glm::vec3 dir_to_camera(cos_theta * sin_phi, sin_theta * sin_phi, cos_phi);
	return (dir_to_camera * cam_rel_pos_sphere_.x) + camera_target_;
	*/
	glm::vec3 player_car_direction = player_car_->get_direction();
	glm::vec3 left_direction = glm::normalize(glm::cross(player_car_direction, up_vector_));
	return camera_target_ + (player_car_direction * -0.3f) + (up_vector_ * 0.2f) + (left_direction * 0.1f);
}

glm::mat4 BezierGame::calc_world_to_camera_matrix(const glm::vec3 &camera_position, const glm::vec3 &camera_target, const glm::vec3 &up_vector)
{
	glm::vec3 look_dir = glm::normalize(camera_target - camera_position);
	glm::vec3 up_dir = glm::normalize(up_vector);

	//glm::vec3 right_dir = glm::normalize(glm::cross(look_dir, up_dir));
	glm::vec3 right_dir = glm::normalize(glm::cross(up_dir, look_dir));
	//glm::vec3 perp_up_dir = glm::cross(right_dir, look_dir);
	glm::vec3 perp_up_dir = glm::cross(look_dir, right_dir);

	glm::mat4 rot_matrix(1.0f);
	rot_matrix[0] = glm::vec4(right_dir, 0.0f);
	rot_matrix[1] = glm::vec4(perp_up_dir, 0.0f);
	rot_matrix[2] = glm::vec4(-look_dir, 0.0f);

	rot_matrix = glm::transpose(rot_matrix);

	glm::mat4 trans_matrix(1.0f);
	trans_matrix[3] = glm::vec4(-camera_position, 1.0f);

	return rot_matrix * trans_matrix;
}

void BezierGame::display()
{
	for (unsigned int i = 0; i < moving_game_objects_.size(); ++i) {
		moving_game_objects_[i]->update();
	}
	for (unsigned int i = 0; i < non_moving_game_objects_.size(); ++i) {
		non_moving_game_objects_[i]->update();
	}
	check_collision();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 camera_position = resolve_camera_position();
	glutil::MatrixStack world_to_camera_matrix;
	camera_target_ = player_car_->get_position();
	world_to_camera_matrix.SetMatrix(calc_world_to_camera_matrix(camera_position, camera_target_, up_vector_));
	//world_to_camera_matrix.LookAt(camera_position, camera_target_, up_vector_);

	glutil::MatrixStack model_matrix;
	model_matrix.SetMatrix(camera_to_clip_matrix_ * world_to_camera_matrix.Top());
	
	for (unsigned int i = 0; i < moving_game_objects_.size(); ++i) {
		moving_game_objects_[i]->display(model_matrix);
	}
	for (unsigned int i = 0; i < non_moving_game_objects_.size(); ++i) {
		non_moving_game_objects_[i]->display(model_matrix);
	}

	glutSwapBuffers();
	glutPostRedisplay();
}

void BezierGame::reshape(int width, int height)
{
	glutil::MatrixStack perspective_matrix;
	perspective_matrix.Perspective(50.0f, width / (float)height, z_near_, z_far_);
	camera_to_clip_matrix_ = perspective_matrix.Top();

	glViewport(0, 0, width, height);
	glutPostRedisplay();
}

void BezierGame::keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		glutLeaveMainLoop();
		return;
		
	case 'w': player_car_->forward(); break;
	case 's': player_car_->backward(); break;
	case 'a': player_car_->left(); break;
	case 'd': player_car_->right(); break;
	/*
	case 'w': camera_target_.z += 0.04f; break;
	case 's': camera_target_.z -= 0.04f; break;
	case 'd': camera_target_.x += 0.04f; break;
	case 'a': camera_target_.x -= 0.04f; break;
	case 'e': camera_target_.y += 0.04f; break;
	case 'q': camera_target_.y -= 0.04f; break;
	case 'W': camera_target_.z += 0.004f; break;
	case 'S': camera_target_.z -= 0.004f; break;
	case 'D': camera_target_.x += 0.004f; break;
	case 'A': camera_target_.x -= 0.004f; break;
	case 'E': camera_target_.y += 0.004f; break;
	case 'Q': camera_target_.y -= 0.004f; break;
	case 'i': cam_rel_pos_sphere_.y -= 11.25f; break;
	case 'k': cam_rel_pos_sphere_.y += 11.25f; break;
	case 'j': cam_rel_pos_sphere_.z -= 11.25f; break;
	case 'l': cam_rel_pos_sphere_.z += 11.25f; break;
	case 'o': cam_rel_pos_sphere_.x -= 0.5f; break;
	case 'u': cam_rel_pos_sphere_.x += 0.5f; break;
	case 'I': cam_rel_pos_sphere_.y -= 1.125f; break;
	case 'K': cam_rel_pos_sphere_.y += 1.125f; break;
	case 'J': cam_rel_pos_sphere_.z -= 1.125f; break;
	case 'L': cam_rel_pos_sphere_.z += 1.125f; break;
	case 'O': cam_rel_pos_sphere_.x -= 0.05f; break;
	case 'U': cam_rel_pos_sphere_.x += 0.05f; break;
	*/
	}
	//cam_rel_pos_sphere.y = glm::clamp(cam_rel_pos_sphere.y, -78.75f, -1.0f);
	//camera_target.y = camera_target.y > 0.0f ? camera_target.y : 0.0f;
	//cam_rel_pos_sphere.z = cam_rel_pos_sphere.z > 5.0f ? cam_rel_pos_sphere.z : 5.0f;
	//glutPostRedisplay();
}

int BezierGame::check_plane_versus_bounding_sphere(BSP_Node *bsp_node, glm::vec3 center, float radius)
{
	//std::cout << "check plane versus bounding sphere funtcion begin" << std::endl;
	float d = bsp_node->plane_function(center);
	int flag = 0;
	if (d - radius <= 0.0f) flag |= BSP_IN_BACK;
	if (d + radius >= 0.0f) flag |= BSP_IN_FRONT;
	return flag;
}

bool BezierGame::axis_separates_polygons(glm::vec3 &separating_direction, PolygonFace *first_polygon, PolygonFace *second_polygon)
{
	//std::cout << "axis separates polygons function begin" << std::endl;
	separating_direction = glm::normalize(separating_direction);
	/*
	std::cout << "separating_direction";
	print_vector(separating_direction);
	std::cout << "first polygon:" << std::endl;
	std::cout << "\t"; print_vector(*(first_polygon->get_vertex(0)));
	std::cout << "\t"; print_vector(*(first_polygon->get_vertex(1)));
	std::cout << "\t"; print_vector(*(first_polygon->get_vertex(2)));
	std::cout << "second polygon:" << std::endl;
	std::cout << "\t"; print_vector(*(second_polygon->get_vertex(0)));
	std::cout << "\t"; print_vector(*(second_polygon->get_vertex(1)));
	std::cout << "\t"; print_vector(*(second_polygon->get_vertex(2)));
	*/
	float first_min = std::numeric_limits<float>::max();
	float second_min = std::numeric_limits<float>::max();
	float first_max = -std::numeric_limits<float>::max();
	float second_max = -std::numeric_limits<float>::max();
	for (int i = 0; i < first_polygon->get_vertex_count(); ++i) {
		float projection = glm::dot(separating_direction, *(first_polygon->get_vertex(i)));
		if (projection < first_min)
			first_min = projection;
		if (projection > first_max)
			first_max = projection;
	}
	for (int i = 0; i < second_polygon->get_vertex_count(); ++i) {
		float projection = glm::dot(separating_direction, *(second_polygon->get_vertex(i)));
		if (projection < second_min)
			second_min = projection;
		if (projection > second_max)
			second_max = projection;
	}
	if (first_min > second_max || first_max < second_min) {
		//std::cout << "axis separates polygons" << std::endl;
		return true;
	}
	else {
		//std::cout << "axis does not separate polygons" << std::endl;
		return false;
	}
}

bool BezierGame::polygon_lies_in_plane(PolygonFace *polygon, float a, float b, float c, float d)
{
	//std::cout << "polygon lies in plane function begin" << std::endl;
	for (int i = 0; i < polygon->get_vertex_count(); ++i) {
		glm::vec3 vertex = *(polygon->get_vertex(i));
		if (a * vertex.x + b * vertex.y + c * vertex.z + d != 0.0f)
			return false;
	}
	return true;
}

bool BezierGame::polygons_intersect(PolygonFace *first_polygon, PolygonFace *second_polygon)
{
	//std::cout << "polygons intersect function begin" << std::endl;
	glm::vec3 first_normal = glm::cross(*(first_polygon->get_vertex(1)) - *(first_polygon->get_vertex(0)), *(first_polygon->get_vertex(2)) - *(first_polygon->get_vertex(0)));
	if (axis_separates_polygons(first_normal, first_polygon, second_polygon)) {
		//std::cout << "first normal separates polygons" << std::endl;
		return false;
	}
	glm::vec3 second_normal = glm::cross(*(second_polygon->get_vertex(1)) - *(second_polygon->get_vertex(0)), *(second_polygon->get_vertex(2)) - *(second_polygon->get_vertex(0)));
	if (axis_separates_polygons(second_normal, first_polygon, second_polygon)) {
		//std::cout << "second normal separates polygons" << std::endl;
		return false;
	}
	//std::cout << "normals do not separate polygons" << std::endl;
	if (polygon_lies_in_plane(first_polygon, second_normal.x, second_normal.y, second_normal.z, glm::dot(-1.0f * second_normal, *(second_polygon->get_vertex(0))))) {
		//std::cout << "polygons are coplanar" << std::endl;
		for (int i = 1; i < first_polygon->get_vertex_count(); ++i) {
			if (axis_separates_polygons(glm::cross(first_normal, *(first_polygon->get_vertex(i)) - *(first_polygon->get_vertex(i - 1))), first_polygon, second_polygon)) {
				//std::cout << "first polygon's edge number " << i << " normal separates polygons" << std::endl;
				return false;
			}
		}
		if (axis_separates_polygons(glm::cross(first_normal, *(first_polygon->get_vertex(0)) - *(first_polygon->get_vertex(first_polygon->get_vertex_count() - 1))), first_polygon, second_polygon)) {
			//std::cout << "first polygon's edge number " << first_polygon->get_vertex_count() << " normal separates polygons" << std::endl;
			return false;
		}
		for (int i = 1; i < second_polygon->get_vertex_count(); ++i) {
			if (axis_separates_polygons(glm::cross(second_normal, *(second_polygon->get_vertex(i)) - *(second_polygon->get_vertex(i - 1))), first_polygon, second_polygon)) {
				//std::cout << "second polygon's edge number " << i << " normal separates polygons" << std::endl;
				return false;
			}
		}
		if (axis_separates_polygons(glm::cross(second_normal, *(second_polygon->get_vertex(0)) - *(second_polygon->get_vertex(second_polygon->get_vertex_count() - 1))), first_polygon, second_polygon)) {
			//std::cout << "second polygon's edge number " << second_polygon->get_vertex_count() << " normal separates polygons" << std::endl;
			return false;
		}
		//std::cout << "polygons are coplanar and intersect" << std::endl;
		return true;
	}
	else {
		//std::cout << "polygons are not coplanar" << std::endl;
		for (int i = 1; i < first_polygon->get_vertex_count() + 1; ++i) {
			//std::cout << "i: " << i << std::endl;
			glm::vec3 first_edge;
			if (i == first_polygon->get_vertex_count()) {
				first_edge = *(first_polygon->get_vertex(0)) - *(first_polygon->get_vertex(first_polygon->get_vertex_count() - 1));
			}
			else {
				first_edge = *(first_polygon->get_vertex(i)) - *(first_polygon->get_vertex(i - 1));
			}
			for (int j = 1; j < second_polygon->get_vertex_count() + 1; ++j) {
				//std::cout << "j: " << j << std::endl;
				glm::vec3 second_edge;
				if (j == second_polygon->get_vertex_count()) {
					second_edge = *(second_polygon->get_vertex(0)) - *(second_polygon->get_vertex(second_polygon->get_vertex_count() - 1));
				}
				else {
					second_edge = *(second_polygon->get_vertex(j)) - *(second_polygon->get_vertex(j - 1));
				}
				if (axis_separates_polygons(glm::cross(first_edge, second_edge), first_polygon, second_polygon)) {
					//std::cout << "cross product of first polygon's edge number " << i << " and second polygon's edge number " << j << " separates polygons" << std::endl;
					return false;
				}
			}
		}
		//std::cout << "polygons are not coplanar and intersect" << std::endl;
		return true;
	}
	std::cout << "Error in polygons intersect, none of exclusive cases were executed" << std::endl;
	return false;
}

bool BezierGame::objects_intersect_bsp(BSP_Node *bsp_node, GameObject *object)
{
	//std::cout << "objects intersect bsp function begin" << std::endl;
	if (bsp_node == 0) return false;
	int bsp_flag = check_plane_versus_bounding_sphere(bsp_node, object->get_center(), object->get_radius());
	if (bsp_flag == BSP_IN_BOTH) {
		PolygonFace object_polygon;
		for (int i = 0; i < bsp_node->get_polygon_count(); ++i) {
			for (int j = 0; j < object->get_polygon_count(); ++j) {
				object->get_polygon(j, object_polygon);
				if (polygons_intersect(bsp_node->get_polygon(i), &object_polygon)) {
					return true;
				}
			}
		}
	}
	if (bsp_flag & BSP_IN_FRONT) {
		if (objects_intersect_bsp(bsp_node->get_front_node(), object))
			return true;
	}
	if (bsp_flag & BSP_IN_BACK) {
		if (objects_intersect_bsp(bsp_node->get_back_node(), object))
			return true;
	}
	return false;
}

bool BezierGame::check_collision_bsp(GameObject *first_object, GameObject *second_object)
{
	//std::cout << "check collision bsp function begin" << std::endl;
	if (first_object->get_radius() >= second_object->get_radius())
		return objects_intersect_bsp(first_object->get_bsp_root_node(), second_object);
	else
		return objects_intersect_bsp(second_object->get_bsp_root_node(), first_object);
}

bool BezierGame::check_bounding_spheres(GameObject *first_object, GameObject *second_object)
{
	//std::cout << "check bounding spheres function begin" << std::endl;
	glm::vec3 distance = first_object->get_center() - second_object->get_center();
	float r = first_object->get_radius() + second_object->get_radius();
	if (glm::dot(distance, distance) < r*r)
		return true;
	return false;
}

void BezierGame::check_collision()
{
	//std::cout << "check collision function begin" << std::endl;
	for (unsigned int i = 0; i < moving_game_objects_.size(); ++i) {
		if (moving_game_objects_[i]->bounding_sphere_check()) {
			for (unsigned int j = i + 1; j < moving_game_objects_.size(); ++j) {
				if (moving_game_objects_[j]->bounding_sphere_check()) {
					if (check_bounding_spheres(moving_game_objects_[i], moving_game_objects_[j])) {
						if (check_collision_bsp(moving_game_objects_[i], moving_game_objects_[j])) {
							std::cout << "Crash " << moving_game_objects_[i]->get_name() << " " << moving_game_objects_[j]->get_name() << std::endl;
							resolve_collision_moving(moving_game_objects_[i], moving_game_objects_[j]);
						}
						else {
							std::cout << "No crash " << moving_game_objects_[i]->get_name() << " " << moving_game_objects_[j]->get_name() << std::endl;
						}
					}
				}
			}
			for (unsigned int j = 0; j < non_moving_game_objects_.size(); ++j) {
				if (non_moving_game_objects_[j]->bounding_sphere_check()) {
					if (check_bounding_spheres(moving_game_objects_[i], non_moving_game_objects_[j])) {
						if (check_collision_bsp(moving_game_objects_[i], non_moving_game_objects_[j])) {
							std::cout << "Crash " << moving_game_objects_[i]->get_name() << " " << non_moving_game_objects_[j]->get_name() << std::endl;
						}
						else {
							std::cout << "No crash " << moving_game_objects_[i]->get_name() << " " << non_moving_game_objects_[j]->get_name() << std::endl;
						}
					}
				}
			}
		}
	}
}

void BezierGame::resolve_collision_moving(GameObject *first_object, GameObject *second_object)
{
	first_object->undo_time_step();
	if (!check_collision_bsp(first_object, second_object)) {
		std::cout << first_object->get_name() << " crashed into " << second_object->get_name() << std::endl;
	}
	else {
		first_object->update();
		second_object->undo_time_step();
		if (!check_collision_bsp(first_object, second_object)) {
			std::cout << second_object->get_name() << " crashed into " << first_object->get_name() << std::endl;
		}
		else {
			std::cout << "THIS CRASH IS TOO LONG!!" << std::endl;
		}
	}
}

glm::vec3 BezierGame::get_dir_to_light()
{
	return dir_to_light_;
}

glm::vec4 BezierGame::get_light_intensity()
{
	return light_intensity_;
}

glm::vec4 BezierGame::get_ambient_intensity()
{
	return ambient_intensity_;
}

void BezierGame::print_matrix(glm::mat4 matrix)
{
	for (unsigned int i = 0; i < 4; ++i) {
		for (unsigned int j = 0; j < 4; ++j) {
			std::cout << matrix[j][i] << " ";
		}
		std::cout << std::endl;
	}
}

void BezierGame::print_matrix(glm::mat3 matrix)
{
	for (unsigned int i = 0; i < 3; ++i) {
		for (unsigned int j = 0; j < 3; ++j) {
			std::cout << matrix[j][i] << " ";
		}
		std::cout << std::endl;
	}
}

void BezierGame::print_vector(glm::vec4 vector)
{
	std::cout << "(" << vector.x << " " << vector.y << " " << vector.z << " " << vector.w << ")" << std::endl;
}

void BezierGame::print_vector(glm::vec3 vector)
{
	std::cout << "(" << vector.x << " " << vector.y << " " << vector.z << ")" << std::endl;
}

int main(int argc, char **argv)
{
	try {
		bezier_game = new BezierGame(argc, argv);
		bezier_game->start();
	}
	catch (int ret) {
		return ret;
	}
	return 0;
}
