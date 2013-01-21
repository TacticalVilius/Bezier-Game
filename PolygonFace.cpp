#include "PolygonFace.h"
#include <iostream>

PolygonFace::PolygonFace()
{
}

PolygonFace::PolygonFace(glm::vec3 vertex)
{
	vertices_.push_back(new glm::vec3(vertex));
}

PolygonFace::PolygonFace(glm::vec3 first_vertex, glm::vec3 second_vertex)
{
	vertices_.push_back(new glm::vec3(first_vertex));
	vertices_.push_back(new glm::vec3(second_vertex));
}

PolygonFace::PolygonFace(glm::vec3 first_vertex, glm::vec3 second_vertex, glm::vec3 third_vertex)
{
	vertices_.push_back(new glm::vec3(first_vertex));
	vertices_.push_back(new glm::vec3(second_vertex));
	vertices_.push_back(new glm::vec3(third_vertex));
}

PolygonFace::PolygonFace(glm::vec3 first_vertex, glm::vec3 second_vertex, glm::vec3 third_vertex, glm::vec3 fourth_vertex)
{
	vertices_.push_back(new glm::vec3(first_vertex));
	vertices_.push_back(new glm::vec3(second_vertex));
	vertices_.push_back(new glm::vec3(third_vertex));
	vertices_.push_back(new glm::vec3(fourth_vertex));
}

PolygonFace::PolygonFace(PolygonFace *polygon)
{
	for (int i = 0; i < polygon->get_vertex_count(); ++i) {
		glm::vec3 *vertex = polygon->get_vertex(i);
		vertices_.push_back(new glm::vec3(vertex->x, vertex->y, vertex->z));
	}
}

PolygonFace::~PolygonFace()
{
	for (unsigned int i = 0; i < vertices_.size(); ++i) {
		delete vertices_[i];
	}
}

void PolygonFace::translate(const glm::vec3 &offset)
{
	for (unsigned int i = 0; i < vertices_.size(); ++i) {
		(*(vertices_[i])) += offset;
	}
}

void PolygonFace::rotate(const glm::mat4 &rotation_matrix)
{
	glm::mat3 rot_mat(rotation_matrix);
	for (unsigned int i = 0; i < vertices_.size(); ++i) {
		(*(vertices_[i])) = rot_mat * (*(vertices_[i]));
	}
}

void PolygonFace::scale(float factor)
{
	for (unsigned int i = 0; i < vertices_.size(); ++i) {
		(*(vertices_[i])) *= factor;
	}
}

void PolygonFace::transform(const glm::mat4 &transformation_matrix)
{
	for (unsigned int i = 0; i < vertices_.size(); ++i) {
		glm::vec4 vertex(*(vertices_[i]), 1.0f);
		vertex = transformation_matrix * vertex;
		vertices_[i]->x = vertex.x;
		vertices_[i]->y = vertex.y;
		vertices_[i]->z = vertex.z;
	}
}

int PolygonFace::get_vertex_count() const
{
	return vertices_.size();
}

glm::vec3* PolygonFace::get_vertex(int index) const
{
	return vertices_[index];
}

void PolygonFace::add_vertex(glm::vec3 *vertex)
{
	vertices_.push_back(vertex);
}

void PolygonFace::remove_vertices()
{
	for (unsigned int i = 0; i < vertices_.size(); ++i) {
		delete vertices_[i];
	}
	vertices_.clear();
}