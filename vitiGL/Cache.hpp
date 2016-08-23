#pragma once

/* 
	static class to load textures (and later maybe other resources) and store them by name and id. 
	no texture will be loaded twice!

	throws a file Error if the path to the texture is invalid or the file could not be opened
*/

#include <string>
#include <map>
#include <GL/glew.h>

namespace vitiGL {

struct slData;

/* cache for shapes xml-data: */
class ShapeCache {
public:
	ShapeCache();
	~ShapeCache();

	slData load(const std::string& path);

private:
	std::map<std::string, slData> _cache;
};

/* cache for textures: */
class TextureCache {
public:
	TextureCache();
	~TextureCache();

	GLuint load(const std::string& path, bool sRGB);

private:
	GLuint loadImage(const std::string& path, bool sRGB);
	std::map<std::string, GLuint> _cache;
};

/* Static wrapper class for access to all caches: */
class Cache {
public:
	static GLuint getTexture(const std::string& path, bool sRGB = false);
	static slData getShape(const std::string& path);
private:
	static TextureCache  _textureCache;
	static ShapeCache	 _shapeCache;
};

}