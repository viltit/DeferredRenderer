#pragma once

#include <glm/glm.hpp>
#include <GL\glew.h>
#include <vector>

#include "Shader.hpp"
#include "vitiTypes.hpp"
#include "ShapeLoader.hpp"


namespace vitiGL {

/* struct for getting data from the ShapeLoader Class: */
struct slData {
	glm::vec3 size;
	glm::vec2 uv;
	std::vector<std::string> textures;
	bool invert;
};


/*	ABSTRACT BASE CLASS IDRAWABLE --------------------------------------------------------------
	
	Task:	Deliver an interface for all drawable object that we can use in the Scene Manager

	------------------------------------------------------------------------------------------ */
class IDrawable {
public:
	IDrawable() {};
	virtual ~IDrawable() {};

	virtual void draw(const Shader& shader) const = 0;
	virtual void drawNaked(const Shader& shader) const { draw(shader); }

	virtual void setModelMatrix(const glm::mat4& P) { M = P; }

protected:
	glm::mat4 M;
};

/*	ABSTRACT BASE CLASS SHAPE -------------------------------------------------------------------
	 
	Task:	Keep all variables and methods needed for drawing textured Meshes with openGL
			Things like scaling, moving, ... should happen in the class that holds the Shape.

	Caveats:	The shader has to be delivered AND activated from within the class that calls 
				the draw command.

	to do:	Implement glInstancedDraw
			Implement element buffers and element drawing
	------------------------------------------------------------------------------------------ */
class Shape : public IDrawable {
public:
	Shape();
	virtual ~Shape();

	virtual void draw(const Shader& shader) const override;
	virtual void drawNaked(const Shader& shader) const override;
	//virtual void multidraw(const Shader& shader) = 0;

	virtual void setModelMatrix(const glm::mat4& P)	override	{ M = P; }
	//virtual glm::mat4 modelMatrix()							{ return M; }

	bool srgbOn()	{ sRGB = true; }
	bool srgbOff()	{ sRGB = false; }

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
	bool		invert;		//invert the normals
	bool		sRGB;		//gamma-correct diffuse textures on loading
};

/*	---------------------- GEOMETRIC FORMS DERIVED FROM SHAPE ------------------------------- */

class Cuboid : public Shape {
public:
	Cuboid(const std::string& configFile, const glm::vec3& position = { 0.0f, 0.0f, 0.0f });
	~Cuboid();

protected:
	virtual void initVertices(std::vector<Vertex>& vertices);

	glm::vec3	size;
	glm::vec2	uv;
};

/* ------------------------------------------------------------------------------------------- */
class Tetrahedron : public Shape {
public:
	Tetrahedron(const std::string& configFile, const glm::vec3& position = { 0.0f, 0.0f, 0.0f });
	~Tetrahedron();

private:
	void initVertices(std::vector<Vertex>& vertices) override;

	glm::vec3	size;
	glm::vec2	uv;
};

/* ------------------------------------------------------------------------------------------- */
class Octahedron : public Shape {
public:
	Octahedron(const std::string& config_file, const glm::vec3& position = { 0.0f, 0.0f, 0.0f });
	~Octahedron();

private:
	void initVertices(std::vector<Vertex>& vertices) override;

	glm::vec3	size;
	glm::vec2	uv;
};

/* ------------------------------------------------------------------------------------------- */
class Icosahedron : public Shape {
public:
	Icosahedron(const std::string& config_file, const glm::vec3& position = { 0.0f, 0.0f, 0.0f });
	~Icosahedron();

protected:
	Icosahedron() : Shape() {}	/* for sub-class sphere */
	void initVertices(std::vector<Vertex>& vertices) override;

	glm::vec3	size;
	glm::vec2	uv;

};

/* ONLY USE FOR POINT LIGHT DRAWING FOR NOW (the normals are very wrong!) ---------------------- */
class Sphere : public Icosahedron {
public:
	Sphere(const std::string& config_file);
	~Sphere();

private:
	/* subdivide the icosahedrons triangles to form a sphere: */
	void subdivide(std::vector<Vertex>& vertices, float size);

	/*	function for refining the triangles, ie find the middle and move the middle point
	away from the spheres center */
	glm::vec3 middle_pos(const glm::vec3& point_a, const glm::vec3& point_b, float size);
	glm::vec2 middle_uv(const glm::vec2& uv_a, const glm::vec2& uv_b);
};

}
