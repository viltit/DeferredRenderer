#include "Cache.hpp"

#include <fstream>
#include <src/SOIL.h>
#include "Error.hpp"

namespace vitiGL {

//redeclare static member:
TextureCache Cache::_textureCache;

TextureCache::TextureCache() {
}

TextureCache::~TextureCache() {
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

//simple wrapper function:
GLuint Cache::getTexture(const std::string& path, bool sRGB) {
	return _textureCache.load(path, sRGB);
}

}