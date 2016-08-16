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

	glUniform1f(shader.getUniform("material.shiny"), 22.0f);

	//give the textures to the shader:
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tbo[0]);
	glUniform1i(shader.getUniform("material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tbo[1]);
	glUniform1i(shader.getUniform("material.specular"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tbo[2]);
	glUniform1i(shader.getUniform("material.normal"), 2);

	glUniform1f(shader.getUniform("material.shiny"), 33.0f);

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

void Shape::setPos(const glm::vec3& pos) {
	M[3][0] = pos.x;
	M[3][1] = pos.y;
	M[3][2] = pos.z;
}

void Shape::rotate(float angle, const glm::vec3& axis) {
	M = glm::rotate(M, glm::radians(angle), axis);
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

Cuboid::Cuboid(const std::string& configFile, const glm::vec3& position)
	:	Shape()
{
	slData data;
	ShapeLoader loader(data, configFile);
	
	size = data.size;
	uv = data.uv;

	tbo.push_back(Cache::getTexture(data.textures[0]));
	tbo.push_back(Cache::getTexture(data.textures[1]));
	tbo.push_back(Cache::getTexture(data.textures[2]));

	setPos(position);

	std::vector<Vertex> vertices;
	initVertices(vertices);
	calcNormals(vertices);
	calcTangents(vertices);
	uploadVertices(vertices);
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

}