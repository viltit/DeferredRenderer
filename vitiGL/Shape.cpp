#include "Shape.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <iostream>
#include <unordered_map>

#include "Cache.hpp"
#include "Shader.hpp"

namespace vitiGL {

Shape::Shape() 
	:	IGameObject { ObjType::shape },
		vao			{ 0 },
		vbo			{ 0 },
		numVertices	{ 0 },
		invert		{ false },
		sRGB		{ true }
{}


Shape::~Shape() {
	//if (vao) glDeleteVertexArrays(1, &vao);
	//if (vbo) glDeleteBuffers(1, &vbo);
}

void Shape::draw(const Shader& shader) const {
	//give the model matrix to the shader:
	glUniformMatrix4fv(shader.getUniform("M"), 1, GL_FALSE, glm::value_ptr(_M));

	//give the textures to the shader:
	material.setUniforms(shader);

	//draw:
	glBindVertexArray(vao);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, numVertices);
	glBindVertexArray(0);
}

void Shape::drawNaked(const Shader & shader) const {
	glUniformMatrix4fv(shader.getUniform("M"), 1, GL_FALSE, glm::value_ptr(_M));

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, numVertices);
	glBindVertexArray(0);
}

void Shape::uploadVertices(const std::vector<Vertex>& vertices) {
	glCreateVertexArrays(1, &vao);		//May crash without grafic card -> maybe use gen instead
	glCreateBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glBindVertexArray(0);
}

void Shape::calcNormals(std::vector<Vertex>& vertices) {
	for (int i = 0; i < vertices.size();) {
		//get the next triangle:
		Vertex& v0 = vertices[i++];
		Vertex& v1 = vertices[i++];
		Vertex& v2 = vertices[i++];

		//Calculate two edges of the triangle:
		glm::vec3 e1 = v1.pos - v0.pos;
		glm::vec3 e2 = v2.pos - v1.pos;

		//get the normal with the cross product:
		glm::vec3 normal = glm::normalize(glm::cross(e1, e2));

		v0.normal = normal;
		v1.normal = normal;
		v2.normal = normal;
	}
	//to do: average the different normals of overlapping edges
}

void Shape::calcTangents(std::vector<Vertex>& vertices, bool bitangents) {
	/* see http://ogldev.atspace.co.uk/www/tutorial26/tutorial26.html for the math behind this function */

	for (int i = 0; i < vertices.size();) {

		/* get the next triangle: */
		Vertex& v0 = vertices[i++];
		Vertex& v1 = vertices[i++];
		Vertex& v2 = vertices[i++];

		/* calculate two edges in the triangle: */
		glm::vec3 e1 = v1.pos - v0.pos;
		glm::vec3 e2 = v2.pos - v0.pos;

		/* calculate the uv-distances: */
		float delta_u1 = v1.uv.x - v0.uv.x;
		float delta_v1 = v1.uv.y - v0.uv.y;
		float delta_u2 = v2.uv.x - v0.uv.x;
		float delta_v2 = v2.uv.y - v0.uv.y;

		float f = 1.0f / (delta_u1 * delta_v2 - delta_u2 * delta_v1);

		glm::vec3 tangent;
		glm::vec3 bitangent;

		/* calculate tangent: */
		tangent.x = f * (delta_v2 * e1.x - delta_v1 * e2.x);
		tangent.y = f * (delta_v2 * e1.y - delta_v1 * e2.y);
		tangent.z = f * (delta_v2 * e1.z - delta_v1 * e2.z);

		v0.tangent += tangent;
		v1.tangent += tangent;
		v2.tangent += tangent;

		/* calculate bitangent if wanted: */
		if (bitangents) {
			bitangent.x = f * (-delta_u2 * e1.x - delta_u1 * e2.x);
			bitangent.y = f * (-delta_u2 * e1.y - delta_u1 * e2.y);
			bitangent.z = f * (-delta_u2 * e1.z - delta_u1 * e2.z);

			v0.bitangent += bitangent;
			v1.bitangent += bitangent;
			v2.bitangent += bitangent;
		}
	}

	/* normalize the tangents (together with the += above, we get an average): */
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i].tangent = glm::normalize(vertices[i].tangent);
		/* same for the bitangent, if wanted: */
		if (bitangents) {
			vertices[i].bitangent = glm::normalize(vertices[i].bitangent);
		}
	}
}


/*	-----------------------------------------------------------------------------------------------------
CLASS SHAPEI FOR INDEXED DRAWING
----------------------------------------------------------------------------------------------------- */

ShapeI::ShapeI() 
	:	IGameObject	{ ObjType::shape },
		vao			{ 0 },
		vbo			{ 0 },
		ebo			{ 0 },
		numVertices	{ 0 }
{}

ShapeI::ShapeI(ObjType type)
	:	IGameObject	{ type },
		vao			{ 0 },
		vbo			{ 0 },
		ebo			{ 0 },
		numVertices	{ 0 }
{}

ShapeI::~ShapeI() {
}

void ShapeI::draw(const Shader & shader) const {
	//give the model matrix to the shader:
	glUniformMatrix4fv(shader.getUniform("M"), 1, GL_FALSE, glm::value_ptr(_M));

	//glUniform1f(shader.getUniform("material.shiny"), 22.0f);

	//give the textures to the shader:
	material.setUniforms(shader);

	//draw:
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void ShapeI::drawNaked(const Shader & shader) const {
	glUniformMatrix4fv(shader.getUniform("M"), 1, GL_FALSE, glm::value_ptr(_M));

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void ShapeI::uploadVertices(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices) {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	/* upload vertex data: */
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	/* upload index data: */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

	/* tell openGL how to interpret the vertex data: */
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	glBindVertexArray(0);
}

void ShapeI::calcNormals(std::vector<Vertex>& vertices, std::vector<GLuint>& indices) {
	for (size_t i = 0; i < indices.size();) {
		Vertex& v0 = vertices[indices[i++]];
		Vertex& v1 = vertices[indices[i++]];
		Vertex& v2 = vertices[indices[i++]];

		//Calculate two edges of the triangle:
		glm::vec3 e1 = v1.pos - v0.pos;
		glm::vec3 e2 = v2.pos - v1.pos;

		//get the normal with the cross product:
		glm::vec3 normal = glm::normalize(glm::cross(e1, e2));

		v0.normal += normal;
		v1.normal += normal;
		v2.normal += normal;
	}

	//normalize the normals (we should get flat shading this way)
	for (size_t i = 0; i < vertices.size(); i++) {
		vertices[i].normal = glm::normalize(vertices[i].normal);
	}
}

void ShapeI::calcTangents(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, bool bitangents) {
	/* see http://ogldev.atspace.co.uk/www/tutorial26/tutorial26.html for the math behind this function */

	for (size_t i = 0; i < indices.size();) {

		/* get the next triangle: */
		Vertex& v0 = vertices[indices[i++]];
		Vertex& v1 = vertices[indices[i++]];
		Vertex& v2 = vertices[indices[i++]];

		/* calculate two edges in the triangle: */
		glm::vec3 e1 = v1.pos - v0.pos;
		glm::vec3 e2 = v2.pos - v0.pos;

		/* calculate the uv-distances: */
		float delta_u1 = v1.uv.x - v0.uv.x;
		float delta_v1 = v1.uv.y - v0.uv.y;
		float delta_u2 = v2.uv.x - v0.uv.x;
		float delta_v2 = v2.uv.y - v0.uv.y;

		float f = 1.0f / (delta_u1 * delta_v2 - delta_u2 * delta_v1);

		glm::vec3 tangent;
		glm::vec3 bitangent;

		/* calculate tangent: */
		tangent.x = f * (delta_v2 * e1.x - delta_v1 * e2.x);
		tangent.y = f * (delta_v2 * e1.y - delta_v1 * e2.y);
		tangent.z = f * (delta_v2 * e1.z - delta_v1 * e2.z);

		v0.tangent += tangent;
		v1.tangent += tangent;
		v2.tangent += tangent;

		/* calculate bitangent if wanted: */
		if (bitangents) {
			bitangent.x = f * (-delta_u2 * e1.x - delta_u1 * e2.x);
			bitangent.y = f * (-delta_u2 * e1.y - delta_u1 * e2.y);
			bitangent.z = f * (-delta_u2 * e1.z - delta_u1 * e2.z);

			v0.bitangent += bitangent;
			v1.bitangent += bitangent;
			v2.bitangent += bitangent;
		}
	}

	/* normalize the tangents (together with the += above, we get an average): */
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i].tangent = glm::normalize(vertices[i].tangent);
		/* same for the bitangent, if wanted: */
		if (bitangents) {
			vertices[i].bitangent = glm::normalize(vertices[i].bitangent);
		}
	}
}

void ShapeI::normalizeSeam(std::vector<Vertex>& vertices, std::vector<GLuint>& indices) {
	/*	we still have a problem with vertices at the object's seam
	(they share the same position but not the same uv coordiants and still have different normals,
	which leads to a visible edge on the seam)

	SO, heres a hacky solution for that:
	*/
	std::vector<std::pair<GLuint, GLuint>> sharedInd;
	std::unordered_map<glm::vec3, GLuint> uniquePos;

	for (size_t i = 0; i < indices.size(); i++) {
		glm::vec3 pos = vertices[indices[i]].pos;

		if (uniquePos.count(pos) == 0) {	//no other vertex shares the same position yet
			uniquePos[pos] = indices[i];
		}
		else {								//we have two vertices sharing the same pos
			Vertex& v0 = vertices[indices[i]];
			Vertex& v1 = vertices[uniquePos[pos]];

			if (indices[i] != uniquePos[pos]) {
				glm::vec3 normal = glm::normalize(v0.normal + v1.normal);
				v0.normal = normal;
				v1.normal = normal;
				glm::vec3 tangent = glm::normalize(v0.tangent + v1.tangent);
			}
		}
	}
}

/*	-----------------------------------------------------------------------------------------------------
	GEOMETRIC FORMS DERIVED FROM SHAPE
	----------------------------------------------------------------------------------------------------- */

Cuboid::Cuboid(const std::string& configFile, const glm::vec3& position)
	:	Shape()
{
	VertexData vdata{ 0, 0, 0 };
	slData sdata = Cache::getShape(configFile);

	size = sdata.size;
	uv = sdata.uv;

	material.setTexture(TEXTURE_DIFFUSE, Cache::getTexture(sdata.textures[0], sRGB));
	material.setTexture(TEXTURE_SPECULAR, Cache::getTexture(sdata.textures[1]));
	material.setTexture(TEXTURE_NORMAL, Cache::getTexture(sdata.textures[2]));

	if (!Cache::isVertexLoaded(configFile)) {
		std::vector<Vertex> vertices;
		initVertices(vertices);
		calcNormals(vertices);
		calcTangents(vertices);
		uploadVertices(vertices);
		numVertices = vertices.size();
		Cache::pushVertex(configFile, vao, vbo, numVertices);
	}
	else {
		vdata = Cache::pullVertex(configFile);
		vao = vdata.vao;
		vbo = vdata.vbo;
		numVertices = vdata.numVertices;
	}

	_M = glm::translate(_M, position);
}

Cuboid::~Cuboid() {
	//if (vao) glDeleteVertexArrays(1, &vao);
	//if (vbo) glDeleteBuffers(1, &vbo);
}

void Cuboid::initVertices(std::vector<Vertex>& vertices) {
	//we will need this variables at a later point:
	float w = size.x / 2.0f;
	float h = size.y / 2.0f;
	float d = size.z / 2.0f;

	float uv_w = uv.x;
	float uv_h = uv.y;

	
	vertices.resize(36);

	vertices[numVertices].pos = glm::vec3{ -w, -h,  d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };	/* front */
	vertices[numVertices].pos = glm::vec3{ w, -h,  d };		vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };
	vertices[numVertices].pos = glm::vec3{ w,  h,  d };		vertices[numVertices++].uv = glm::vec2{ uv_w, uv_h };
	vertices[numVertices].pos = glm::vec3{ w,  h,  d };		vertices[numVertices++].uv = glm::vec2{ uv_w, uv_h };
	vertices[numVertices].pos = glm::vec3{ -w,  h,  d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, uv_h };
	vertices[numVertices].pos = glm::vec3{ -w, -h,  d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };

	vertices[numVertices].pos = glm::vec3{ -w,  h,  d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, uv_h };	/* left */
	vertices[numVertices].pos = glm::vec3{ -w,  h, -d };	vertices[numVertices++].uv = glm::vec2{ uv_w, uv_h };
	vertices[numVertices].pos = glm::vec3{ -w, -h, -d };	vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };
	vertices[numVertices].pos = glm::vec3{ -w, -h, -d };	vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };
	vertices[numVertices].pos = glm::vec3{ -w, -h,  d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };
	vertices[numVertices].pos = glm::vec3{ -w,  h,  d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, uv_h };

	vertices[numVertices].pos = glm::vec3{ w,  h, -d };		vertices[numVertices++].uv = glm::vec2{ uv_w, uv_h };	/* back */
	vertices[numVertices].pos = glm::vec3{ w, -h, -d };		vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };
	vertices[numVertices].pos = glm::vec3{ -w, -h, -d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };
	vertices[numVertices].pos = glm::vec3{ -w, -h, -d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };
	vertices[numVertices].pos = glm::vec3{ -w,  h, -d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, uv_h };
	vertices[numVertices].pos = glm::vec3{ w,  h, -d };		vertices[numVertices++].uv = glm::vec2{ uv_w, uv_h };

	vertices[numVertices].pos = glm::vec3{ w, -h, -d };		vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };	/* right */
	vertices[numVertices].pos = glm::vec3{ w,  h, -d };		vertices[numVertices++].uv = glm::vec2{ uv_w, uv_h };
	vertices[numVertices].pos = glm::vec3{ w,  h,  d };		vertices[numVertices++].uv = glm::vec2{ 0.0f, uv_h };
	vertices[numVertices].pos = glm::vec3{ w,  h,  d };		vertices[numVertices++].uv = glm::vec2{ 0.0f, uv_h };
	vertices[numVertices].pos = glm::vec3{ w, -h,  d };		vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };
	vertices[numVertices].pos = glm::vec3{ w, -h, -d };		vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };

	vertices[numVertices].pos = glm::vec3{ w,  h,  d };		vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };	/* top */
	vertices[numVertices].pos = glm::vec3{ w,  h, -d };		vertices[numVertices++].uv = glm::vec2{ uv_w, uv_h };
	vertices[numVertices].pos = glm::vec3{ -w,  h, -d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, uv_h };
	vertices[numVertices].pos = glm::vec3{ -w,  h, -d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, uv_h };
	vertices[numVertices].pos = glm::vec3{ -w,  h,  d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };
	vertices[numVertices].pos = glm::vec3{ w,  h,  d };		vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };

	vertices[numVertices].pos = glm::vec3{ -w, -h, -d };	vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };	/* bottom */
	vertices[numVertices].pos = glm::vec3{ w, -h, -d };		vertices[numVertices++].uv = glm::vec2{ uv_w, uv_h };
	vertices[numVertices].pos = glm::vec3{ w, -h,  d };		vertices[numVertices++].uv = glm::vec2{ 0.0f, uv_h };
	vertices[numVertices].pos = glm::vec3{ w, -h,  d };		vertices[numVertices++].uv = glm::vec2{ 0.0f, uv_h };
	vertices[numVertices].pos = glm::vec3{ -w, -h,  d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };
	vertices[numVertices].pos = glm::vec3{ -w, -h, -d };	vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };
}

/*	------------------------------------------------------------------------------------------------------------- */
Tetrahedron::Tetrahedron(const std::string & configFile, const glm::vec3& position) : Shape() {
	/* load data from config file: */
	VertexData vdata{ 0, 0, 0 };
	slData sdata = Cache::getShape(configFile);

	size = sdata.size;
	uv = sdata.uv;

	material.setTexture(TEXTURE_DIFFUSE, Cache::getTexture(sdata.textures[0], sRGB));
	material.setTexture(TEXTURE_SPECULAR, Cache::getTexture(sdata.textures[1]));
	material.setTexture(TEXTURE_NORMAL, Cache::getTexture(sdata.textures[2]));

	if (!Cache::isVertexLoaded(configFile)) {
		std::vector<Vertex> vertices;
		initVertices(vertices);
		calcNormals(vertices);
		calcTangents(vertices);
		uploadVertices(vertices);
		numVertices = vertices.size();
		Cache::pushVertex(configFile, vao, vbo, numVertices);
	}
	else {
		vdata = Cache::pullVertex(configFile);
		vao = vdata.vao;
		vbo = vdata.vbo;
		numVertices = vdata.numVertices;
	}

	_M = glm::translate(_M, position);
}

Tetrahedron::~Tetrahedron() {
	//if (vao) glDeleteVertexArrays(1, &vao);
	//if (vbo) glDeleteBuffers(1, &vbo);
}

void Tetrahedron::initVertices(std::vector<Vertex>& vertices) {
	float w = size.x  * 0.5f;
	float h = size.y * 0.5f;
	float d = size.z * 0.5f;

	float uv_w = uv.x;
	float uv_h = uv.y;

	numVertices = 0;

	vertices.resize(12);

	vertices[numVertices].pos = glm::vec3{ -w, h, -d };		vertices[numVertices++].uv = glm::vec2{ 0.75*uv_w, 0.5*uv_h };	/* A-B-C */
	vertices[numVertices].pos = glm::vec3{ -w, -h, d };		vertices[numVertices++].uv = glm::vec2{ 0.5*uv_w, 0.0f };
	vertices[numVertices].pos = glm::vec3{ w, h, d };		vertices[numVertices++].uv = glm::vec2{ 0.25*uv_w, 0.5*uv_h };

	vertices[numVertices].pos = glm::vec3{ -w, -h, d };		vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };			/* B-D-C */
	vertices[numVertices].pos = glm::vec3{ w, -h, -d };		vertices[numVertices++].uv = glm::vec2{ 0.5*uv_w, 0.0f };
	vertices[numVertices].pos = glm::vec3{ w, h, d };		vertices[numVertices++].uv = glm::vec2{ 0.25*uv_w, 0.5*uv_h };

	vertices[numVertices].pos = glm::vec3{ -w, h, -d };		vertices[numVertices++].uv = glm::vec2{ 0.75*uv_w, 0.5*uv_h };	/* A-C-D */
	vertices[numVertices].pos = glm::vec3{ w, h, d };		vertices[numVertices++].uv = glm::vec2{ 0.25*uv_w, 0.5*uv_h };
	vertices[numVertices].pos = glm::vec3{ w, -h, -d };		vertices[numVertices++].uv = glm::vec2{ 0.5*uv_w, 0.0f };

	vertices[numVertices].pos = glm::vec3{ -w, h, -d };		vertices[numVertices++].uv = glm::vec2{ 0.75*uv_w, 0.5*uv_h };	/* A-D-B */
	vertices[numVertices].pos = glm::vec3{ w, -h, -d };		vertices[numVertices++].uv = glm::vec2{ 0.5*uv_w, 0.0f };
	vertices[numVertices].pos = glm::vec3{ -w, -h, d };		vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };
}

/*	------------------------------------------------------------------------------------------------------------- */

Octahedron::Octahedron(const std::string& configFile, const glm::vec3& position) : Shape() {
	
	VertexData vdata{ 0, 0, 0 };
	slData sdata = Cache::getShape(configFile);

	size = sdata.size;
	uv = sdata.uv;

	material.setTexture(TEXTURE_DIFFUSE, Cache::getTexture(sdata.textures[0], sRGB));
	material.setTexture(TEXTURE_SPECULAR, Cache::getTexture(sdata.textures[1]));
	material.setTexture(TEXTURE_NORMAL, Cache::getTexture(sdata.textures[2]));

	if (!Cache::isVertexLoaded(configFile)) {
		std::vector<Vertex> vertices;
		initVertices(vertices);
		calcNormals(vertices);
		calcTangents(vertices);
		uploadVertices(vertices);
		numVertices = vertices.size();
		Cache::pushVertex(configFile, vao, vbo, numVertices);
	}
	else {
		vdata = Cache::pullVertex(configFile);
		vao = vdata.vao;
		vbo = vdata.vbo;
		numVertices = vdata.numVertices;
	}

	_M = glm::translate(_M, position);
}

Octahedron::~Octahedron() {
	//if (vao) glDeleteVertexArrays(1, &vao);
	//if (vbo) glDeleteBuffers(1, &vbo);
}

void Octahedron::initVertices(std::vector<Vertex>& vertices) {
	float w = size.x  * 0.5f;
	float h = size.y * 0.5f;
	float d = size.z * 0.5f;

	float uv_w = uv.x;
	float uv_h = uv.y;

	vertices.resize(24);
	numVertices = 0;


	/*		   F				lazy try to visualize the octahedron
			. /\.
		  .  /  \  .
		D.  /	 \   .E
		   A	  C
			\	 /
			 \   /
			  \ /
			   B
	*/

	vertices[numVertices].pos = glm::vec3{ -w, 0.0f, 0.0f }; vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };	/* D-A-F */
	vertices[numVertices].pos = glm::vec3{ 0.0f, 0.0f, d };	vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };
	vertices[numVertices].pos = glm::vec3{ 0.0f, h, 0.0f };	vertices[numVertices++].uv = glm::vec2{ uv_w / 2.0f, uv_h };

	vertices[numVertices].pos = glm::vec3{ 0.0f, 0.0f, d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };	/* A-C-F */
	vertices[numVertices].pos = glm::vec3{ w, 0.0f, 0.0f };	vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };
	vertices[numVertices].pos = glm::vec3{ 0.0, h, 0.0f };	vertices[numVertices++].uv = glm::vec2{ uv_w / 2.0f, uv_h };

	vertices[numVertices].pos = glm::vec3{ 0.0f, 0.0f, d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };	/* A-B-C */
	vertices[numVertices].pos = glm::vec3{ 0.0f, -h, 0.0f }; vertices[numVertices++].uv = glm::vec2{ uv_w / 2.0f, uv_h };
	vertices[numVertices].pos = glm::vec3{ w, 0.0f, 0.0f };	vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };

	vertices[numVertices].pos = glm::vec3{ 0.0f, h, 0.0f };	vertices[numVertices++].uv = glm::vec2{ uv_w / 2.0f, uv_h };/* F-C-E*/
	vertices[numVertices].pos = glm::vec3{ w, 0.0f, 0.0f };	vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };
	vertices[numVertices].pos = glm::vec3{ 0.0f, 0.0f, -d }; vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };

	vertices[numVertices].pos = glm::vec3{ -w, 0.0f, 0.0f }; vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };	/* D-F-E */
	vertices[numVertices].pos = glm::vec3{ 0.0f, h, 0.0f };	vertices[numVertices++].uv = glm::vec2{ uv_w / 2.0f, uv_h };
	vertices[numVertices].pos = glm::vec3{ 0.0f, 0.0f, -d }; vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };

	vertices[numVertices].pos = glm::vec3{ -w, 0.0f, 0.0f }; vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };	/* D-B-A */
	vertices[numVertices].pos = glm::vec3{ 0.0f, -h, 0.0f }; vertices[numVertices++].uv = glm::vec2{ uv_w / 2.0f, uv_h };
	vertices[numVertices].pos = glm::vec3{ 0.0f, 0.0f, d };	vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };

	vertices[numVertices].pos = glm::vec3{ 0.0f, -h, 0.0f }; vertices[numVertices++].uv = glm::vec2{ uv_w / 2.0f, uv_h };	/* B-E-C */
	vertices[numVertices].pos = glm::vec3{ 0.0f, 0.0, -d };	vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };
	vertices[numVertices].pos = glm::vec3{ w, 0.0f, 0.0 };	vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };

	vertices[numVertices].pos = glm::vec3{ -w, 0.0f, 0.0f };	vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };	/* D-E-B*/
	vertices[numVertices].pos = glm::vec3{ 0.0f, 0.0f, -d };	vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };
	vertices[numVertices].pos = glm::vec3{ 0.0f, -h, 0.0f };	vertices[numVertices++].uv = glm::vec2{ uv_w / 2.0f, uv_h };
}

/*	------------------------------------------------------------------------------------------------------------- */

Icosahedron::Icosahedron(const std::string & configFile, const glm::vec3& position)
	: Shape()
{
	/* load data from config file: */
	VertexData vdata{ 0, 0, 0 };
	slData sdata = Cache::getShape(configFile);

	size = sdata.size;
	uv = sdata.uv;

	material.setTexture(TEXTURE_DIFFUSE, Cache::getTexture(sdata.textures[0], sRGB));
	material.setTexture(TEXTURE_SPECULAR, Cache::getTexture(sdata.textures[1]));
	material.setTexture(TEXTURE_NORMAL, Cache::getTexture(sdata.textures[2]));

	if (!Cache::isVertexLoaded(configFile)) {
		std::vector<Vertex> vertices;
		initVertices(vertices);
		calcNormals(vertices);
		calcTangents(vertices);
		uploadVertices(vertices);
		numVertices = vertices.size();
		Cache::pushVertex(configFile, vao, vbo, numVertices);
	}
	else {
		vdata = Cache::pullVertex(configFile);
		vao = vdata.vao;
		vbo = vdata.vbo;
		numVertices = vdata.numVertices;
	}

	_M = glm::translate(_M, position);
}

Icosahedron::~Icosahedron() {
	//if (vao) glDeleteVertexArrays(1, &vao);
	//if (vbo) glDeleteBuffers(1, &vbo);
}

void Icosahedron::initVertices(std::vector<Vertex>& vertices) {
	/*	 kudos to this two pages for constructing the vertices:
	http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
	http://blog.coredumping.com/subdivision-of-icosahedrons/					*/

	float w = size.x / 2.0f;
	float t = w * (1.0f + sqrt(5.0f)) / 2.0f;	/* golden ratio */
	float uv_w = uv.x;
	float uv_h = uv.y;

	int id = 0;

	/* define the edges */
	std::vector<Vertex> edges(12);
	edges.resize(12);
	edges[id++].pos = glm::vec3{ -w, t, 0.0f };
	edges[id++].pos = glm::vec3{ w, t, 0.0f };
	edges[id++].pos = glm::vec3{ -w, -t, 0.0f };
	edges[id++].pos = glm::vec3{ w, -t, 0.0f };

	edges[id++].pos = glm::vec3{ 0.0f, -t, w };
	edges[id++].pos = glm::vec3{ 0.0f, w, t };
	edges[id++].pos = glm::vec3{ 0.0f, -w, -t };
	edges[id++].pos = glm::vec3{ 0.0f, w, -t };

	edges[id++].pos = glm::vec3{ t, 0.0f, -w };
	edges[id++].pos = glm::vec3{ t, 0.0f, w };
	edges[id++].pos = glm::vec3{ -t, 0.0f, -w };
	edges[id++].pos = glm::vec3{ -t, 0.0f, w };

	/* define the indices for triangle drawing: */
	id = 0;
	std::vector<GLuint> indices(60);
	indices[id++] = 0; indices[id++] = 11; indices[id++] = 5;
	indices[id++] = 0; indices[id++] = 5; indices[id++] = 1;
	indices[id++] = 0; indices[id++] = 1; indices[id++] = 7;
	indices[id++] = 0; indices[id++] = 7; indices[id++] = 10;
	indices[id++] = 0; indices[id++] = 10; indices[id++] = 11;

	indices[id++] = 1; indices[id++] = 5; indices[id++] = 9;
	indices[id++] = 5; indices[id++] = 11; indices[id++] = 4;
	indices[id++] = 11; indices[id++] = 10; indices[id++] = 2;
	indices[id++] = 10; indices[id++] = 7; indices[id++] = 6;
	indices[id++] = 7; indices[id++] = 1; indices[id++] = 8;

	indices[id++] = 3; indices[id++] = 9; indices[id++] = 4;
	indices[id++] = 3; indices[id++] = 4; indices[id++] = 2;
	indices[id++] = 3; indices[id++] = 2; indices[id++] = 6;
	indices[id++] = 3; indices[id++] = 6; indices[id++] = 8;
	indices[id++] = 3; indices[id++] = 8; indices[id++] = 9;

	indices[id++] = 4; indices[id++] = 9; indices[id++] = 5;
	indices[id++] = 2; indices[id++] = 4; indices[id++] = 11;
	indices[id++] = 6; indices[id++] = 2; indices[id++] = 10;
	indices[id++] = 8; indices[id++] = 6; indices[id++] = 7;
	indices[id++] = 9; indices[id++] = 8; indices[id++] = 1;

	vertices.resize(60);
	for (int i = 0; i < 60; i++) {
		vertices[i] = edges[indices[i]];
	}

	/* now we calculate the uv coordinates: */
	id = 0;
	float pw = 5.5f;
	float ph = 3.0f;
	std::vector<glm::vec2> uv(22);

	uv[0] = glm::vec2(uv_w * 0.5f / pw, 0);
	uv[1] = glm::vec2(uv_w * 1.5f / pw, 0);
	uv[2] = glm::vec2(uv_w * 2.5f / pw, 0);
	uv[3] = glm::vec2(uv_w * 3.5f / pw, 0);
	uv[4] = glm::vec2(uv_w * 4.5f / pw, 0);

	uv[5] = glm::vec2(0, uv_h / ph);
	uv[6] = glm::vec2(uv_w / pw, uv_h / ph);
	uv[7] = glm::vec2(2.0f * uv_w / pw, uv_h / ph);
	uv[8] = glm::vec2(3.0f * uv_w / pw, uv_h / ph);
	uv[9] = glm::vec2(4.0f * uv_w / pw, uv_h / ph);
	uv[10] = glm::vec2(5.0f * uv_w / pw, uv_h / ph);

	uv[11] = glm::vec2(0.5f * uv_w / pw, 2.0f * uv_h / ph);
	uv[12] = glm::vec2(1.5f * uv_w / pw, 2.0f * uv_h / ph);
	uv[13] = glm::vec2(2.5f * uv_w / pw, 2.0f * uv_h / ph);
	uv[14] = glm::vec2(3.5f * uv_w / pw, 2.0f * uv_h / ph);
	uv[15] = glm::vec2(4.5f * uv_w / pw, 2.0f * uv_h / ph);
	uv[16] = glm::vec2(1.0f * uv_w, 2.0f * uv_h / ph);

	uv[17] = glm::vec2(1.0f * uv_w / pw, uv_h);
	uv[18] = glm::vec2(2.0f * uv_w / pw, uv_h);
	uv[19] = glm::vec2(3.0f * uv_w / pw, uv_h);
	uv[20] = glm::vec2(4.0f * uv_w / pw, uv_h);
	uv[21] = glm::vec2(5.0f * uv_w / pw, uv_h);

	//first row
	vertices[id++].uv = uv[0];	vertices[id++].uv = uv[5];	vertices[id++].uv = uv[6];
	vertices[id++].uv = uv[1];	vertices[id++].uv = uv[6];	vertices[id++].uv = uv[7];
	vertices[id++].uv = uv[2];	vertices[id++].uv = uv[7];	vertices[id++].uv = uv[8];
	vertices[id++].uv = uv[3];	vertices[id++].uv = uv[8];	vertices[id++].uv = uv[9];
	vertices[id++].uv = uv[4];	vertices[id++].uv = uv[9];	vertices[id++].uv = uv[10];

	//second row
	vertices[id++].uv = uv[7];	vertices[id++].uv = uv[6];	vertices[id++].uv = uv[12];
	vertices[id++].uv = uv[6];	vertices[id++].uv = uv[5];	vertices[id++].uv = uv[11];
	vertices[id++].uv = uv[10];	vertices[id++].uv = uv[9];	vertices[id++].uv = uv[15];
	vertices[id++].uv = uv[9];	vertices[id++].uv = uv[8];	vertices[id++].uv = uv[14];
	vertices[id++].uv = uv[8];	vertices[id++].uv = uv[7];	vertices[id++].uv = uv[13];

	//fourth row
	vertices[id++].uv = uv[17];	vertices[id++].uv = uv[12];	vertices[id++].uv = uv[11];
	vertices[id++].uv = uv[21];	vertices[id++].uv = uv[16];	vertices[id++].uv = uv[15];
	vertices[id++].uv = uv[20];	vertices[id++].uv = uv[15];	vertices[id++].uv = uv[14];
	vertices[id++].uv = uv[19];	vertices[id++].uv = uv[14];	vertices[id++].uv = uv[13];
	vertices[id++].uv = uv[18];	vertices[id++].uv = uv[13];	vertices[id++].uv = uv[12];

	//third row
	vertices[id++].uv = uv[11];	vertices[id++].uv = uv[12];	vertices[id++].uv = uv[6];
	vertices[id++].uv = uv[15];	vertices[id++].uv = uv[16];	vertices[id++].uv = uv[10];
	vertices[id++].uv = uv[14];	vertices[id++].uv = uv[15];	vertices[id++].uv = uv[9];
	vertices[id++].uv = uv[13];	vertices[id++].uv = uv[14];	vertices[id++].uv = uv[8];
	vertices[id++].uv = uv[12];	vertices[id++].uv = uv[13];	vertices[id++].uv = uv[7];
}

Sphere::Sphere(const std::string & configFile) : Icosahedron() {
	/* load data from config file: */
	VertexData vdata{ 0, 0, 0 };
	slData sdata = Cache::getShape(configFile);

	size = sdata.size;
	uv = sdata.uv;

	material.setTexture(TEXTURE_DIFFUSE, Cache::getTexture(sdata.textures[0], sRGB));
	material.setTexture(TEXTURE_SPECULAR, Cache::getTexture(sdata.textures[1]));
	material.setTexture(TEXTURE_NORMAL, Cache::getTexture(sdata.textures[2]));

	if (!Cache::isVertexLoaded(configFile)) {
		std::vector<Vertex> vertices;
		initVertices(vertices);

		/* refine the vertices to form a sphere: */
		for (int i = 0; i < 5; i++) {
			subdivide(vertices, size.x * 0.5f);
		}
		calcNormals(vertices);
		calcTangents(vertices);
		uploadVertices(vertices);
		numVertices = vertices.size();
		Cache::pushVertex(configFile, vao, vbo, numVertices);
	}
	else {
		vdata = Cache::pullVertex(configFile);
		vao = vdata.vao;
		vbo = vdata.vbo;
		numVertices = vdata.numVertices;
	}
}

Sphere::~Sphere() {
	//if (vao) glDeleteVertexArrays(1, &vao);
	//if (vbo) glDeleteBuffers(1, &vbo);
}

void Sphere::subdivide(std::vector<Vertex>& vertices, float size) {
	std::vector<Vertex> new_vertices;
	new_vertices.resize(4 * vertices.size());
	Vertex knot_a;
	Vertex knot_b;
	Vertex knot_c;

	int j = 0;
	for (int i = 0; i < vertices.size(); i = i + 3) {
		knot_a.pos = middle_pos(vertices[i].pos, vertices[i + 1].pos, size);
		knot_b.pos = middle_pos(vertices[i + 1].pos, vertices[i + 2].pos, size);
		knot_c.pos = middle_pos(vertices[i + 2].pos, vertices[i].pos, size);

		knot_a.uv = middle_uv(vertices[i].uv, vertices[i + 1].uv);
		knot_b.uv = middle_uv(vertices[i + 1].uv, vertices[i + 2].uv);
		knot_c.uv = middle_uv(vertices[i + 2].uv, vertices[i].uv);

		/* 1st. triangle: */
		new_vertices[j++] = knot_c;
		new_vertices[j++] = vertices[i];
		new_vertices[j++] = knot_a;

		/* 2nd. triangle: */
		new_vertices[j++] = knot_a;
		new_vertices[j++] = vertices[i + 1];
		new_vertices[j++] = knot_b;

		/* 3rd triangle: */
		new_vertices[j++] = knot_b;
		new_vertices[j++] = vertices[i + 2];
		new_vertices[j++] = knot_c;

		/* 4th triangle: */
		new_vertices[j++] = knot_c;
		new_vertices[j++] = knot_a;
		new_vertices[j++] = knot_b;
	}
	vertices = new_vertices;
}


glm::vec3 Sphere::middle_pos(const glm::vec3 & point_a, const glm::vec3 & point_b, float size) {
	float t = (1.0f + sqrt(5.0f)) / 2.0f;					/* golden ratio */
	glm::vec3 temp = (point_b - point_a) * 0.5f + point_a;	/* middle point */

															/* add offset to form a sphere: */
	temp = glm::normalize(temp);
	temp *= sqrt(t*t + 1.0) * size;

	return temp;
}

glm::vec2 Sphere::middle_uv(const glm::vec2& uv_a, const glm::vec2& uv_b) {
	return (uv_b - uv_a) * 0.5f + uv_a;		/* plain middle point: */
}

}