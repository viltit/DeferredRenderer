#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace vitiGEO {

struct hEdge;
struct hFace;

struct hVertex {
	int id;
	glm::vec3 pos;
	std::vector<int> edges;
	std::vector<int> faces;
};

struct hEdge {
	int id;
	int vertexStart;
	int vertexEnd;
	std::vector<int> neighbourEdges;
	std::vector<int> faces;
};

struct hFace {
	int id;
	glm::vec3 normal;
	std::vector<int> vertices;
	std::vector<int> edges;
	std::vector<int> neighbourFaces;
};

class Hull {
public:
	Hull();
	~Hull();

	void addVertex(const glm::vec3& v);
	void addFace(const glm::vec3& normal, int numVertices, const std::vector<int> vertIds);

	int edgeID(int v1_ID, int v2_ID);

	const hVertex& vertex(int id)	{ return _vertices[id]; }
	const hEdge& edge(int id)		{ return _edges[id]; }
	const hFace& face(int id)		{ return _faces[id]; }

	size_t numVertices()			{ return _vertices.size(); }
	size_t numEdges()				{ return _edges.size(); }
	size_t numFaces()				{ return _faces.size(); }

protected:
	int addEdge(int parentFace, int vStart, int vEnd);	/* called by addFace */

	std::vector<hVertex>	_vertices;
	std::vector<hEdge>		_edges;
	std::vector<hFace>		_faces;

};
}
