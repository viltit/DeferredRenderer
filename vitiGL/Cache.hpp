#pragma once

/* 
	static class to load textures, xml-shapes, and vertex array objects, and store them in a 
	std::map by name.

	Goal: load and store resources only one time

	May throw a vitiGL::fileError

	TO DO:		Add an option to delete resources
	TO IMPROVE: ? Get rid of static wrapper class? (template base class?)
*/

#include <string>
#include <map>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <AABB.hpp>
#include "Model.hpp"

namespace vitiGL {

struct slData;

/*	Model cache ---------------------------------------------------------------- */
struct ModelData {
	std::vector<std::vector<Vertex>> vertices;
	std::vector<std::vector<GLuint>> indices;
	std::vector<std::vector<std::pair<int, GLuint>>> textures;
};


class ModelCache {
public:
	ModelCache();
	~ModelCache();

	bool isLoaded		(const std::string& modelPath);

	ModelData& pull		(const std::string& modelPath);
	void push			(const std::string& modelPath, ModelData model);

private:
	std::map<std::string, ModelData> _cache;
};


/* Vertex buffer objects and vertex arrray objects: -------------------------- */

struct VertexData {
	VertexData(GLuint Vao, GLuint Vbo, int numVert, vitiGEO::AABB Aabb, const std::vector<glm::vec3> Vertices)
		: vao{ Vao }, vbo{ Vbo }, numVertices { numVert }, aabb{ Aabb }, vertices{ Vertices }
	{}

	GLuint	vao;
	GLuint	vbo;
	int		numVertices;
	vitiGEO::AABB aabb;
	
	std::vector<glm::vec3> vertices;
};

class VertexCache {
public:
	VertexCache();
	~VertexCache();

	bool		isLoaded	(const std::string& meshName);

	VertexData	pull		(const std::string& meshName);
	void		push		(const std::string& meshName, VertexData vertexData);
	
	void		push		(
		const std::string& meshName, 
		GLuint vao, GLuint vbo, 
		int numVertices, 
		vitiGEO::AABB aabb,
		const std::vector<glm::vec3>& Vertices);

private:
	std::map<std::string, VertexData> _cache;
};

/* Shapes xml-data: ------------------------------------------------------------ */
class ShapeCache {
public:
	ShapeCache();
	~ShapeCache();

	slData load(const std::string& path);

private:
	std::map<std::string, slData> _cache;
};

/* Textures: ------------------------------------------------------------------ */
class TextureCache {
public:
	TextureCache();
	~TextureCache();

	GLuint load(const std::string& path, bool sRGB);

private:
	GLuint loadImage(const std::string& path, bool sRGB);
	std::map<std::string, GLuint> _cache;
};

/* Static wrapper class for access to all caches: ----------------------------- */
class Cache {
public:
	static GLuint getTexture(const std::string& path, bool sRGB = false);
	static slData getShape(const std::string& path);

	static bool	isVertexLoaded(const std::string& meshName);
	static VertexData pullVertex(const std::string& meshName);

	static void pushVertex(const std::string & meshName, GLuint vao, GLuint vbo, 
		int numVertices, vitiGEO::AABB aabb, const std::vector<glm::vec3>& Vertices);

	static bool			isModelLoaded(const std::string& modelPath);
	static ModelData	pullModel(const std::string& modelPath);
	static void			pushModel(const std::string& modelPath, ModelData model);

private:
	static TextureCache  _textureCache;
	static ShapeCache	 _shapeCache;
	static VertexCache	 _vertexCache;
	static ModelCache	 _modelCache;
};

}