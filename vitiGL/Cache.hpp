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

#include <AABB.hpp>

namespace vitiGL {

struct slData;

/* Vertex buffer objects and vertex arrray objects: -------------------------- */

struct VertexData {
	VertexData(GLuint Vao, GLuint Vbo, int numVert, vitiGEO::AABB Aabb) 
		: vao{ Vao }, vbo{ Vbo }, numVertices { numVert }, aabb{ Aabb }
	{}

	GLuint	vao;
	GLuint	vbo;
	int		numVertices;
	vitiGEO::AABB aabb;
};

class VertexCache {
public:
	VertexCache();
	~VertexCache();

	bool		isLoaded	(const std::string& meshName);

	VertexData	pull		(const std::string& meshName);
	void		push		(const std::string& meshName, VertexData vertexData);
	void		push		(const std::string& meshName, GLuint vao, GLuint vbo, int numVertices, vitiGEO::AABB aabb);

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
	static void pushVertex(const std::string & meshName, GLuint vao, GLuint vbo, int numVertices, vitiGEO::AABB aabb);

private:
	static TextureCache  _textureCache;
	static ShapeCache	 _shapeCache;
	static VertexCache	 _vertexCache;
};

}