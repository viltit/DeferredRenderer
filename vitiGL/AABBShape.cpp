#include "AABBShape.hpp"

#include "Camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace vitiGL {

AABBShape::AABBShape(const vitiGEO::AABB* box, Camera* cam)
	:	_box{ box },
		_cam{ cam },
		_shader{ "Shaders/DrawShape/vertex.glsl", "Shaders/DrawShape/fragment.glsl" }
{
	std::vector<Vertex> vertices(8);
	std::vector<GLuint> indices(36);
	initVertices(vertices);

	indices = { 
		0, 1, 2, 1, 3, 2,	//front face
		0, 2, 6, 0, 6, 4,	//right face
		1, 7, 3, 1, 5, 7,	//left face
		2, 7, 3, 2, 6, 7,	//bottom face
		5, 1, 0, 5, 0, 4,	//top face,
		5, 6, 7, 5, 4, 6	//back face
	};

	calcNormals(vertices, indices);
	calcTangents(vertices, indices);
	uploadVertices(vertices, indices);

	numVertices = 36;
}

AABBShape::AABBShape(const AABBShape &s) 
	:	_box{ s._box },
		_cam{ s._cam },
		_shader{ "Shaders/DrawShape/vertex.glsl", "Shaders/DrawShape/fragment.glsl" }
{
	numVertices = s.numVertices;
	vao = s.vao;
	vbo = s.vbo;
	ebo = s.ebo;
}


AABBShape::~AABBShape() {
}

void AABBShape::draw() const {
	_shader.on();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	_cam->setVPUniform(_shader);
	
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	_shader.off();
}

void AABBShape::update(vitiGEO::AABB * box) {
	_box = box;
	std::vector<Vertex> vertices(8);
	initVertices(vertices);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(), vertices.data());
	glBindVertexArray(0);
}

void vitiGL::AABBShape::initVertices(std::vector<Vertex>& vertices) {
	for (int i = 0; i <= 7; i++) {
		glm::vec3 pos{};
		pos = _box->corner(i);

		std::cout << "AABB Corner " << i << " :" << pos << std::endl;

		vertices[i].pos = pos;
	}
}

}
