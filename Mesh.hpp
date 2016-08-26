#pragma once

#include <GL/glew.h>
#include <vector>
#include <map>
#include <string>

#include "Shader.hpp"
#include "vitiTypes.hpp"

/*
namespace vitiGL {

struct vertexHandler {
	GLuint vbo;
	GLuint vao;
	//GLuint ebo;
};

class MeshCache {
public:
	MeshCache();
	~MeshCache();
	void load(const std::string& meshName);

private:
	static std::map<std::string, vertexHandler> _meshCache;
};

class Mesh {
public:
	Mesh();
	virtual ~Mesh();

	virtual void draw(const Shader& shader) const;

protected:
	virtual void initVertices(std::vector<Vertex>& vertices);
	virtual void uploadVertices(const std::vector<Vertex>& vertices);
	
	virtual void calcNormals(std::vector<Vertex>& vertices);
	virtual void calcTangents(std::vector<Vertex>& vertices, bool bitangents = true);

	GLuint	_vao;
	GLuint	_vbo;

	int		numVertices;
	bool	invert;
};
}*/
