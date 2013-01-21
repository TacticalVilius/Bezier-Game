#include "GameObject.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

GameObject::GameObject(BezierGame *bezier_game)
	: bezier_game_(bezier_game),
	  name_(""),
	  bsp_root_(0),
	  position_(0.0f, 0.0f, 0.0f),
	  scale_(1.0f),
	  bsp_polygon_count_(0),
	  velocity_(0.0f, 0.0f, 0.0f),
	  mass_(0.0f)
{
}

GameObject::GameObject(BezierGame *bezier_game, std::string name)
	: bezier_game_(bezier_game),
	  name_(name),
	  bsp_root_(0),
	  position_(0.0f, 0.0f, 0.0f),
	  scale_(1.0f),
	  bsp_polygon_count_(0),
	  velocity_(0.0f, 0.0f, 0.0f),
	  mass_(0.0f)
{
}

GameObject::~GameObject()
{
	delete bsp_root_;
	for (int i = 0; i < get_polygon_count(); ++i) {
		delete polygons_[i];
	}
}

void GameObject::do_time_step()
{
	if (moving_object()) {
		delete bsp_root_;
		bsp_root_ = build_bsp_tree();
	}
}

void GameObject::undo_time_step()
{
	if (moving_object()) {
		delete bsp_root_;
		bsp_root_ = build_bsp_tree();
	}
}

GLuint GameObject::load_shader(GLenum shader_type, const std::string &shader_file_name)
{
	try {
		std::ifstream shader_file(shader_file_name.c_str());
		std::stringstream shader_data;
		shader_data << shader_file.rdbuf();
		return glutil::CompileShader(shader_type, shader_data.str());
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		throw;
	}
}

GLuint GameObject::create_program(std::vector<GLuint> shader_list)
{
	try {
		GLuint prog = glutil::LinkProgram(shader_list);
		std::for_each(shader_list.begin(), shader_list.end(), glDeleteShader);
		return prog;
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		std::for_each(shader_list.begin(), shader_list.end(), glDeleteShader);
		throw;
	}
}

ProgramData GameObject::load_program(const std::string &vertex_shader_file_name, const std::string &fragment_shader_file_name)
{
	std::vector<GLuint> shader_list;
	shader_list.push_back(load_shader(GL_VERTEX_SHADER, vertex_shader_file_name));
	shader_list.push_back(load_shader(GL_FRAGMENT_SHADER, fragment_shader_file_name));

	ProgramData program_data;
	program_data.program = create_program(shader_list);
	program_data.model_to_clip_mat_unif = glGetUniformLocation(program_data.program, "model_to_clip_matrix");
	program_data.normal_model_to_world_mat_unif = glGetUniformLocation(program_data.program, "normal_model_to_world_matrix");
	program_data.dir_to_light_unif = glGetUniformLocation(program_data.program, "dir_to_light");
	program_data.light_intensity_unif = glGetUniformLocation(program_data.program, "light_intensity");
	program_data.ambient_intensity_unif = glGetUniformLocation(program_data.program, "ambient_intensity");
	return program_data;
}

int GameObject::check_vector_against_plane(BSP_Node *plane, glm::vec3 vector)
{
	float d = plane->plane_function(vector);
	if (d > 0)
		return BSP_IN_FRONT;
	else if (d < 0)
		return BSP_IN_BACK;
	else
		return 0;
}

void GameObject::split_polygon_with_plane(const BSP_Node &plane, const PolygonFace &polygon, PolygonFace &front_polygon, PolygonFace &back_polygon)
{
	glm::vec3 begin_vertex = *(polygon.get_vertex(polygon.get_vertex_count() - 1));
	float func_begin_vertex = plane.plane_function(begin_vertex);
	for (int i = 0; i < polygon.get_vertex_count(); ++i) {
		glm::vec3 end_vertex = *(polygon.get_vertex(i));
		float func_end_vertex = plane.plane_function(end_vertex);
		if (func_begin_vertex < 0.0f) {
			if (func_end_vertex < 0.0f) {
				back_polygon.add_vertex(new glm::vec3(end_vertex));
			}
			else {
				glm::vec3 *intersection_vertex = new glm::vec3(plane.intersect_with_line(begin_vertex, end_vertex));
				back_polygon.add_vertex(intersection_vertex);
				front_polygon.add_vertex(new glm::vec3(*intersection_vertex));
				front_polygon.add_vertex(new glm::vec3(end_vertex));
			}
		}
		else {
			if (func_end_vertex >= 0) {
				front_polygon.add_vertex(new glm::vec3(end_vertex));
			}
			else {
				glm::vec3 *intersection_vertex = new glm::vec3(plane.intersect_with_line(begin_vertex, end_vertex));
				back_polygon.add_vertex(intersection_vertex);
				back_polygon.add_vertex(new glm::vec3(end_vertex));
				front_polygon.add_vertex(new glm::vec3(*intersection_vertex));
			}
		}
		begin_vertex = end_vertex;
		func_begin_vertex = func_end_vertex;
	}
}

void GameObject::make_bsp_node(BSP_Node *node, std::vector<PolygonFace *> polygons)
{
	glm::vec3 vector_one = *(polygons[0]->get_vertex(1)) - *(polygons[0]->get_vertex(0));
	glm::vec3 vector_two = *(polygons[0]->get_vertex(2)) - *(polygons[0]->get_vertex(0));
	glm::vec3 normal = glm::normalize(glm::cross(vector_one, vector_two));
	node->set_a(normal.x);
	node->set_b(normal.y);
	node->set_c(normal.z);
	node->set_d(glm::dot(-1.0f * normal, *(polygons[0]->get_vertex(0))));
	node->add_polygon(polygons[0]);
	bsp_polygon_count_++;

	std::vector<PolygonFace *> front_polygons;
	std::vector<PolygonFace *> back_polygons;
	for (unsigned int i = 1; i < polygons.size(); ++i) {
		int first_vector_flag = check_vector_against_plane(node, *(polygons[i]->get_vertex(0)));
		int second_vector_flag = check_vector_against_plane(node, *(polygons[i]->get_vertex(1)));
		int third_vector_flag = check_vector_against_plane(node, *(polygons[i]->get_vertex(2)));
		int polygon_flag = first_vector_flag | second_vector_flag | third_vector_flag;

		if (polygon_flag == 0) {
			node->add_polygon(polygons[i]);
			bsp_polygon_count_++;
		}
		else if (polygon_flag == BSP_IN_BOTH) {
			PolygonFace *front_polygon = new PolygonFace();
			PolygonFace *back_polygon = new PolygonFace();
			split_polygon_with_plane(*node, *(polygons[i]), *front_polygon, *back_polygon);
			front_polygons.push_back(front_polygon);
			back_polygons.push_back(back_polygon);
			delete polygons[i];
		}
		else if (polygon_flag & BSP_IN_FRONT) {
			front_polygons.push_back(polygons[i]);
		}
		else if (polygon_flag & BSP_IN_BACK) {
			back_polygons.push_back(polygons[i]);
		}
	}
	if (front_polygons.size() != 0) {
		node->set_front_node(new BSP_Node());
		make_bsp_node(node->get_front_node(), front_polygons);
	}
	if (back_polygons.size() != 0) {
		node->set_back_node(new BSP_Node());
		make_bsp_node(node->get_back_node(), back_polygons);
	}
}

BSP_Node* GameObject::build_bsp_tree()
{
	if (get_polygon_count() == 0)
		return 0;
	std::vector<PolygonFace *> polygons;
	for (int i = 0; i < get_polygon_count(); ++i) {
		PolygonFace *polygon = new PolygonFace(get_original_polygon(i));
		update_polygon(*polygon);
		polygons.push_back(polygon);
	}
	BSP_Node *bsp_root = new BSP_Node();
	make_bsp_node(bsp_root, polygons);
	return bsp_root;
}

void GameObject::move(glm::vec3 offset)
{
	position_ += offset;
	update_transformation_matrix();
}

void GameObject::rotate(glm::mat4 rotation)
{
	rotation_.ApplyMatrix(rotation);
	update_transformation_matrix();
}

void GameObject::scale(float factor)
{
	scale_ *= factor;
	center_ *= factor;
	radius_ *= factor;
	update_transformation_matrix();
}

void GameObject::update_transformation_matrix()
{
	transformation_.SetIdentity();
	transformation_.Translate(position_);
	transformation_.ApplyMatrix(rotation_.Top());
	transformation_.Scale(scale_);
}

bool GameObject::moving_object()
{
	return moving_object_;
}

bool GameObject::bounding_sphere_check()
{
	return bounding_sphere_check_;
}

glm::vec3 GameObject::get_center()
{
	return center_ + position_;
}

float GameObject::get_radius()
{
	return radius_;
}

std::string GameObject::get_name()
{
	return name_;
}

glm::vec3 GameObject::get_position()
{
	return position_;
}

PolygonFace* GameObject::get_original_polygon(int index) const
{
	/*
	PolygonFace polygon(polygons_[index]);
	polygon.scale(scale_);
	if (moving_object_) {
		polygon.rotate(rotation_.Top());
		polygon.translate(position_);
	}
	return polygon;
	*/
	return polygons_[index];
}

void GameObject::update_polygon(PolygonFace &polygon) const
{
	/*
	polygon.scale(scale_);
	polygon.rotate(rotation_.Top());
	polygon.translate(position_);
	*/
	polygon.transform(transformation_.Top());
}

void GameObject::get_polygon(int index, PolygonFace &polygon) const
{
	PolygonFace *original_polygon = get_original_polygon(index);
	polygon.remove_vertices();
	for (int i = 0; i < original_polygon->get_vertex_count(); ++i) {
		polygon.add_vertex(new glm::vec3(*(original_polygon->get_vertex(i))));
	}
	update_polygon(polygon);
}

int GameObject::get_polygon_count() const
{
	return polygons_.size();
}

BSP_Node* GameObject::get_bsp_root_node() const
{
	return bsp_root_;
}

glm::vec3 GameObject::get_velocity() const
{
	return velocity_;
}

void GameObject::set_velocity(const glm::vec3 &velocity)
{
	velocity_ = velocity;
}

void GameObject::update_velocity(const glm::vec3 &vector)
{
	velocity_ += vector;
}

float GameObject::get_mass() const
{
	return mass_;
}

void GameObject::set_mass(float mass)
{
	mass_ = mass;
}