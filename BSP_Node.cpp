#include "BSP_Node.h"
#include <iostream>

BSP_Node::BSP_Node()
	: front_node_(0),
	  back_node_(0)
{
}

BSP_Node::BSP_Node(BSP_Node *bsp_node)
{
	for (unsigned int i = 0; i < bsp_node->get_polygon_count(); ++i) {
		polygons_.push_back(new PolygonFace(bsp_node->get_polygon(i)));
	}
	a_ = bsp_node->get_a();
	b_ = bsp_node->get_b();
	c_ = bsp_node->get_c();
	d_ = bsp_node->get_d();
	front_node_ = bsp_node->get_front_node();
	back_node_ = bsp_node->get_back_node();
}

BSP_Node::~BSP_Node()
{
	for (unsigned int i = 0; i < polygons_.size(); ++i) {
		delete polygons_[i];
	}
	delete front_node_;
	delete back_node_;
}

float BSP_Node::plane_function(const glm::vec3 &vector) const
{
	return a_ * vector.x + b_ * vector.y + c_ * vector.z + d_;
}

float BSP_Node::dot_with_normal(const glm::vec3 &vertex) const
{
	return a_ * vertex.x + b_ * vertex.y + c_ * vertex.z;
}

glm::vec3 BSP_Node::intersect_with_line(const glm::vec3 &begin_vertex, const glm::vec3 &end_vertex) const
{
	float time = (-1.0f * d_ - dot_with_normal(begin_vertex)) / (dot_with_normal(end_vertex - begin_vertex));
	return begin_vertex + (end_vertex - begin_vertex) * time;
}

void BSP_Node::transform(const glm::mat4 &transformation_matrix)
{
	for (unsigned int i = 0; i < polygons_.size(); ++i) {
		polygons_[i]->transform(transformation_matrix);
	}
	glm::vec3 transformed_normal = glm::normalize(glm::vec3(transformation_matrix * glm::vec4(a_, b_, c_, 1.0f)));
	a_ = transformed_normal.x;
	b_ = transformed_normal.y;
	c_ = transformed_normal.z;
	d_ = glm::dot(-1.0f * transformed_normal, *(polygons_[0]->get_vertex(0)));
}

float BSP_Node::get_a() const
{
	return a_;
}

void BSP_Node::set_a(float a)
{
	a_ = a;
}

float BSP_Node::get_b() const
{
	return b_;
}

void BSP_Node::set_b(float b)
{
	b_ = b;
}

float BSP_Node::get_c() const
{
	return c_;
}

void BSP_Node::set_c(float c)
{
	c_ = c;
}
float BSP_Node::get_d() const
{
	return d_;
}

void BSP_Node::set_d(float d)
{
	d_ = d;
}

PolygonFace* BSP_Node::get_polygon(int index) const
{
	return polygons_[index];
}

void BSP_Node::add_polygon(PolygonFace *polygon)
{
	polygons_.push_back(polygon);
}

int BSP_Node::get_polygon_count() const
{
	return polygons_.size();
}

BSP_Node* BSP_Node::get_front_node() const
{
	return front_node_;
}

void BSP_Node::set_front_node(BSP_Node *front_node)
{
	front_node_ = front_node;
}

BSP_Node* BSP_Node::get_back_node() const
{
	return back_node_;
}

void BSP_Node::set_back_node(BSP_Node *back_node)
{
	back_node_ = back_node;
}