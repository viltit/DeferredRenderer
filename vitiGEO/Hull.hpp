/*	 ----------------------------------------------------------------
	class to represent shapes for the Physics Engine
	stores vertices, edges and faces, of the shape	
	
	the class is ressource intensive on construction, but 
	should be fast once the instance is built				
	---------------------------------------------------------------- */

#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace vitiGEO {

struct hEdge;
struct hFace;


/* helper structs: ----------------------------------------------- */
struct hVertex {
	hVertex(glm::vec3 p, int i) 
		: pos { p },
		  id  { id }
	{}

	int id;
	glm::vec3 pos;
	std::vector<int> edges;
	std::vector<int> faces;
};

struct hEdge {
	hEdge(int vertexStart, int vertexEnd, int i)
		:	vStart	{ vertexStart },
			vEnd	{ vertexEnd },
			id		{ i }
	{}

	int id;
	int vStart;
	int vEnd;
	std::vector<int> neighbourEdges;
	std::vector<int> faces;
};

struct hFace {
	hFace(const glm::vec3& n, int i)
		:	normal	{ glm::normalize(n) },
			id		{ i }
	{}

	int id;
	glm::vec3 normal;
	std::vector<int> vertices;
	std::vector<int> edges;
	std::vector<int> neighbourFaces;
};

/* class Hull: ---------------------------------------------- */
class Hull {
public:
	Hull();
	~Hull();

	void addVertex(const glm::vec3& v);
	void addFace(const glm::vec3& normal, const std::vector<int>& vertIds);

	/* returns -1 for not found */
	int edgeID(int v1_ID, int v2_ID);

	void minMaxOnAxis(const glm::vec3 & axis, int& outMin, int& outMax) const;

	/* getters: */
	const hVertex& vertex(int id)	{ return _vertices[id]; }
	const hEdge& edge(int id)		{ return _edges[id]; }
	const hFace& face(int id)		{ return _faces[id]; }

	size_t numVertices()			{ return _vertices.size(); }
	size_t numEdges()				{ return _edges.size(); }
	size_t numFaces()				{ return _faces.size(); }

	void drawDebug(const glm::mat4& M);

protected:
	int addEdge(int parentFace, int vStart, int vEnd);	/* called by addFace */

	std::vector<hVertex>	_vertices;
	std::vector<hEdge>		_edges;
	std::vector<hFace>		_faces;

};
}
