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

class TextureCache {
public:
	TextureCache();
	~TextureCache();

	GLuint load(const std::string& path, bool sRGB);

private:
	GLuint loadImage(const std::string& path, bool sRGB);
	std::map<std::string, GLuint> _cache;
};

class Cache {
public:
	static GLuint getTexture(const std::string& path, bool sRGB = false);
private:
	static TextureCache  _textureCache;
};

}