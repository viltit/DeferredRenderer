#include "Cache.hpp"

#include <fstream>
#include <src/SOIL.h>
#include "Error.hpp"

#include "Shape.hpp"
#include "ShapeLoader.hpp"

namespace vitiGL {

//redeclare static members:
TextureCache	Cache::_textureCache;
ShapeCache		Cache::_shapeCache;
VertexCache		Cache::_vertexCache;


/*	VERTEX BUFFERS ------------------------------------------------------------------------ */

VertexCache::VertexCache() {
}

VertexCache::~VertexCache() {
	//TO DO: DELETE ALL
}

bool VertexCache::isLoaded(const std::string & meshName) {
	auto i = _cache.find(meshName);
	if (i != _cache.end()) return true;
	return false;
}

VertexData VertexCache::pull(const std::string & meshName) {
	auto i = _cache.find(meshName);
	if (i == _cache.end()) throw vitiError(("<VertexCache::pull>Trying to access inexisting data (name identifier = " + meshName + ").").c_str());

	return i->second;
}

void VertexCache::push(const std::string & meshName, VertexData vertexData) {
	auto i = _cache.find(meshName);
	if (i != _cache.end()) throw vitiError(("<VertexCache::push>Trying to push already existing data (name identifier = " + meshName + ").").c_str());
	_cache.insert(std::make_pair(meshName, vertexData));
}

void VertexCache::push(const std::string & meshName, GLuint vao, GLuint vbo, int numVertices) {
	VertexData data{ vao, vbo, numVertices };
	push(meshName, data);
}


/*	TEXTURES -------------------------------------------------------------------------------- */

TextureCache::TextureCache() {
}

TextureCache::~TextureCache() {
	//TO DO: DELETE ALL
}

/*	First, look if the wanted texture is already stored. If yes, give back 
	its id, if no, load it from scratch and put it in storage				
	
	Be aware: We do NOT check for identical sRGB						*/

GLuint TextureCache::load(const std::string& path, bool sRGB) {
	auto i = _cache.find(path);
	if (i != _cache.end()) return i->second;

	GLuint id = loadImage(path, sRGB);

	_cache.insert(std::make_pair(path, id));
	return id;
}

GLuint TextureCache::loadImage(const std::string& path, bool sRGB) {
	//check for a valid path:
	std::fstream file{ path };
	if (!file) throw fileError(path.c_str());
	file.close();

	//Load the image with SOIL:
	GLuint id;
	int w{ 0 };
	int h{ 0 };

	unsigned char* image = SOIL_load_image(path.c_str(), &w, &h, 0, SOIL_LOAD_RGBA);

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	if (sRGB) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	}
	//specify how OpenGL should sample this texture:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);		/* repeat on x-Axis*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);		/* repeat on z-Axis*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);

	return id;
}


/*	SHAPES -------------------------------------------------------------------------------------- */

ShapeCache::ShapeCache() {
}

ShapeCache::~ShapeCache() {
}

slData ShapeCache::load(const std::string & path) {
	/* check if the shape has already been loaded: */
	auto i = _cache.find(path);
	if (i != _cache.end()) return i->second;

	/* if not, load it and insert it into the cache: */
	slData data;
	ShapeLoader{ data, path };

	_cache.insert(std::make_pair(path, data));
	return data;
}

/*	WRAPPER -----------------------------------------------------------------------------------  */

GLuint Cache::getTexture(const std::string& path, bool sRGB) {
	return _textureCache.load(path, sRGB);
}

slData Cache::getShape(const std::string & path) {
	return _shapeCache.load(path);
}

bool Cache::isVertexLoaded(const std::string & meshName) {
	return _vertexCache.isLoaded(meshName);
}

VertexData Cache::pullVertex(const std::string & meshName) {
	return _vertexCache.pull(meshName);
}

void Cache::pushVertex(const std::string & meshName, GLuint vao, GLuint vbo, int numVertices) {
	_vertexCache.push(meshName, vao, vbo, numVertices);
}


}