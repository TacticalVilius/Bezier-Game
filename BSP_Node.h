#ifndef BEZIER_GAME_BSP_NODE_KAZAKAUSKAS
#define BEZIER_GAME_BSP_NODE_KAZAKAUSKAS

#include <vector>
#include <glm/glm.hpp>
#include "PolygonFace.h"

enum BSP_Flag
{
	BSP_IN_FRONT = 0x1,
	BSP_IN_BACK = 0x2,
	BSP_IN_BOTH = 0x3
};

class BSP_Node
{
public:
	BSP_Node();
	BSP_Node(BSP_Node *bsp_node);
	~BSP_Node();
	float plane_function(const glm::vec3 &vector) const;
	glm::vec3 intersect_with_line(const glm::vec3 &begin_vertex, const glm::vec3 &end_vertex) const;
	float dot_with_normal(const glm::vec3 &vertex) const;
	void transform(const glm::mat4 &transformation_matrix);

	float get_a() const;
	void set_a(float a);
	float get_b() const;
	void set_b(float b);
	float get_c() const;
	void set_c(float c);
	float get_d() const;
	void set_d(float d);
	PolygonFace* get_polygon(int index) const;
	void add_polygon(PolygonFace *polygon);
	int get_polygon_count() const;
	BSP_Node* get_front_node() const;
	void set_front_node(BSP_Node *front_node);
	BSP_Node* get_back_node() const;
	void set_back_node(BSP_Node *back_node);

private:
	float a_;
	float b_;
	float c_;
	float d_;
	std::vector<PolygonFace *> polygons_;
	BSP_Node *front_node_;
	BSP_Node *back_node_;
};

#endif