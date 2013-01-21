#ifndef BEZIER_GAME_POLYGON_FACE_KAZAKAUSKAS
#define BEZIER_GAME_POLYGON_FACE_KAZAKAUSKAS

#include <vector>
#include <glm/glm.hpp>

class PolygonFace
{
public:
	PolygonFace();
	PolygonFace(glm::vec3 vertex);
	PolygonFace(glm::vec3 first_vertex, glm::vec3 second_vertex);
	PolygonFace(glm::vec3 first_vertex, glm::vec3 second_vertex, glm::vec3 third_vertex);
	PolygonFace(glm::vec3 first_vertex, glm::vec3 second_vertex, glm::vec3 third_vertex, glm::vec3 fourth_vertex);
	PolygonFace(PolygonFace *polygon);
	~PolygonFace();
	void translate(const glm::vec3 &offset);
	void rotate(const glm::mat4 &rotation_matrix);
	void scale(float factor);
	void transform(const glm::mat4 &transformation_matrix);

	int get_vertex_count() const;
	glm::vec3* get_vertex(int index) const;
	void add_vertex(glm::vec3 *vertex);
	void remove_vertices();

private:
	std::vector<glm::vec3 *> vertices_;
};

#endif