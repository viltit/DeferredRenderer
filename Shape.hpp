#pragma once

#include <glm/glm.hpp>
#include <GL\glew.h>
#include <vector>

#include "Shader.hpp"
#include "vitiTypes.hpp"
#include "ShapeLoader.hpp"

/*	
	Base class Shape. All drawable objects should inhert from Shape so
	we can draw everything with a single command.
	Be aware that the shader has to be delivered AND activated from within
	the class that calls the draw command.
*/

namespace vitiGL {

/* struct for getting data from the ShapeLoader Class: */
struct slData {
	glm::vec3 size;
	glm::vec2 uv;
	std::vector<std::string> textures;
	bool invert;
};

class Shape {
public:
	Shape();
	virtual ~Shape();

	virtual void draw(const Shader& shader) const;
	virtual void drawNaked(const Shader& shader) const;
	//virtual void multidraw(const Shader& shader) = 0;

	void setPos(const glm::vec3& position);
	void setPosMatrix(const glm::mat4& P)		{ M = P; }
	void rotate(float angle, const glm::vec3& axis);

protected:
	virtual void initVertices(std::vector<Vertex>& vertices) = 0; //pure abstract function !!
	virtual void uploadVertices(const std::vector<Vertex>& vertices);

	virtual void calcNormals(std::vector<Vertex>& vertices);	 //only works for triangle meshes!
	virtual void calcTangents(std::vector<Vertex>& vertices, bool bitangents = true);

	glm::mat4	M;			//model matrix, ie. holds the position in world space

	GLuint		vao;
	GLuint		vbo;
	std::vector<GLuint> tbo; //diffuse, specular and normal supported for now

	int			numVertices;
	bool		invert;
};

class Cuboid : public Shape {
public:
	Cuboid(const std::string& configFile, const glm::vec3& position = { 0.0f, 0.0f, 0.0f });
	~Cuboid();

protected:
	virtual void initVertices(std::vector<Vertex>& vertices);

	glm::vec3	size;
	glm::vec2	uv;
};

}
