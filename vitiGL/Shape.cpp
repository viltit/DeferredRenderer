#include "Shape.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "Cache.hpp"

namespace vitiGL {

Shape::Shape() 
	:	vao			{ 0 },
		vbo			{ 0 },
		numVertices	{ 0 },
		invert		{ false }
{}


Shape::~Shape() {
	if (vao) glDeleteVertexArrays(1, &vao);
	if (vbo) glDeleteBuffers(1, &vbo);
}

void Shape::draw(const Shader& shader) const {
	//give the model matrix to the shader:
	glUniformMatrix4fv(shader.getUniform("M"), 1, GL_FALSE, glm::value_ptr(M));

	//glUniform1f(shader.getUniform("material.shiny"), 22.0f);

	//give the textures to the shader:
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tbo[0]);
	glUniform1i(shader.getUniform("diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tbo[1]);
	glUniform1i(shader.getUniform("specular"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tbo[2]);
	glUniform1i(shader.getUniform("normal"), 2);

	//draw:
	glBindVertexArray(vao);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, numVertices);
	glBindVertexArray(0);
}

void Shape::drawNaked(const Shader & shader) const {
	glUniformMatrix4fv(shader.getUniform("M"), 1, GL_FALSE, glm::value_ptr(M));

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
	GEOMETRIC FORMS DERIVED FROM SHAPE
	----------------------------------------------------------------------------------------------------- */

Cuboid::Cuboid(const std::string& configFile, const glm::vec3& position)
	:	Shape()
{
	VertexData vdata{ 0, 0, 0 };
	slData sdata = Cache::getShape(configFile);

	size = sdata.size;
	uv = sdata.uv;

	tbo.push_back(Cache::getTexture(sdata.textures[0]));
	tbo.push_back(Cache::getTexture(sdata.textures[1]));
	tbo.push_back(Cache::getTexture(sdata.textures[2]));

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

	M = glm::translate(M, position);
}

Cuboid::~Cuboid() {
	if (vao) glDeleteVertexArrays(1, &vao);
	if (vbo) glDeleteBuffers(1, &vbo);
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

	tbo.push_back(Cache::getTexture(sdata.textures[0]));
	tbo.push_back(Cache::getTexture(sdata.textures[1]));
	tbo.push_back(Cache::getTexture(sdata.textures[2]));

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

	M = glm::translate(M, position);
}

Tetrahedron::~Tetrahedron() {
	if (vao) glDeleteVertexArrays(1, &vao);
	if (vbo) glDeleteBuffers(1, &vbo);
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

	tbo.push_back(Cache::getTexture(sdata.textures[0]));
	tbo.push_back(Cache::getTexture(sdata.textures[1]));
	tbo.push_back(Cache::getTexture(sdata.textures[2]));

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

	M = glm::translate(M, position);
}

Octahedron::~Octahedron() {
	if (vao) glDeleteVertexArrays(1, &vao);
	if (vbo) glDeleteBuffers(1, &vbo);
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

	tbo.push_back(Cache::getTexture(sdata.textures[0]));
	tbo.push_back(Cache::getTexture(sdata.textures[1]));
	tbo.push_back(Cache::getTexture(sdata.textures[2]));

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

	M = glm::translate(M, position);
}

Icosahedron::~Icosahedron() {
	if (vao) glDeleteVertexArrays(1, &vao);
	if (vbo) glDeleteBuffers(1, &vbo);
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

}