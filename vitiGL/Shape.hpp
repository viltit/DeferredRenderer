#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

#include "vitiTypes.hpp"
#include "IGameObject.hpp"
#include "Material.hpp"
#include "Error.hpp"

#include <AABB.hpp>	//from vitiGEO static library


namespace vitiGL {

class Shader;

/* struct for getting data from the ShapeLoader Class: */
struct slData {
	glm::vec3 size;
	glm::vec2 uv;
	std::vector<std::string> textures;
	bool invert;
};

/*	ABSTRACT BASE CLASS SHAPE -------------------------------------------------------------------
	 
	Task:	Keep all variables and methods needed for drawing textured Meshes with openGL
			Things like scaling, moving, ... should happen in the class that holds the Shape.


	Caveats:	The shader has to be delivered AND activated from within the class that calls 
				the draw command.

				For indexed drawing, use class ShapeI

	to do:	Implement glInstancedDraw

	------------------------------------------------------------------------------------------ */
class Shape : public IGameObject {
public:
	Shape(const std::string& configFile, ShapeType sType);
	virtual ~Shape();

	virtual void draw(const Shader& shader) const override;
	virtual void drawNaked(const Shader& shader) const override;
	//virtual void multidraw(const Shader& shader) = 0;

	/* get a pointer to the objects aabb: */
	vitiGEO::AABB* getAABB() { return &aabb; }

	std::vector<glm::vec3>& vertices() { return _vertices; }

	bool srgbOn()	{ sRGB = true; }
	bool srgbOff()	{ sRGB = false; }

	int countVertices() const { return numVertices; }

	bool isTransparent() const { return material.isTransparent(); }

	ShapeType subtype() const  { return sType; }

protected:
	virtual void initVertices(std::vector<Vertex>& vertices) = 0; //pure abstract function !!
	virtual void uploadVertices(const std::vector<Vertex>& vertices);

	virtual void calcNormals(std::vector<Vertex>& vertices);	 //only works for triangle meshes!
	virtual void calcTangents(std::vector<Vertex>& vertices, bool bitangents = true);

	GLuint		vao;
	GLuint		vbo;
	Material	material;

	std::vector<glm::vec3> _vertices;
	vitiGEO::AABB aabb;

	int			numVertices;
	bool		invert;		//invert the normals
	bool		sRGB;		//gamma-correct diffuse textures on loading

	ShapeType 	sType; 		//Shape Type for scene reconstruction 
};


/*	------------------------ CLASS SHAPEI for indexed drawing ------------------------------- */
class ShapeI : public IGameObject {
public:
	ShapeI(const std::string& configFile);
	ShapeI(ObjType type, const std::string& configFile);
	virtual ~ShapeI();

	virtual void draw(const Shader& shader) const override;
	virtual void drawNaked(const Shader& shader) const override;

	vitiGEO::AABB* getAABB() { return &aabb; }

	std::vector<glm::vec3>& vertices() { return _vertices; }

	int countVertices() const { return numVertices; }

	bool isTransparent() const { return material.isTransparent(); }

protected:
	virtual void initVertices(std::vector<Vertex>& vertices) = 0; //pure abstract function !!

	virtual void uploadVertices(const std::vector<Vertex>& vertices, 
								const std::vector<GLuint>& indices);

	virtual void calcNormals	(std::vector<Vertex>& vertices, 
								std::vector<GLuint>& indices);	 //only works for triangle meshes!
	virtual void calcTangents	(std::vector<Vertex>& vertices, 
								std::vector<GLuint>& indices, 
								bool bitangents = true);

	virtual void normalizeSeam	(std::vector<Vertex>& vertices,
								 std::vector<GLuint>& indices);
	
	std::vector<glm::vec3> _vertices;

	GLuint		vao;
	GLuint		vbo;
	GLuint		ebo;

	Material	material;

	vitiGEO::AABB aabb;

	int			numVertices;
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


/* Helper function to convert ShapeType enum class in a string and vice verca: --------------- */
std::string shapeTypeToString(ShapeType type);

ShapeType stringToShapeType(const std::string& name);

}
