#include "Hull.hpp"

#include "../vitiGL/glRendererDebug.hpp"	//BAD, just to look if drenderer works

namespace vitiGEO {

Hull::Hull() {
}

Hull::~Hull() {
}

void Hull::addVertex(const glm::vec3 & v) {
	hVertex vertex{ v, int(_vertices.size()) };
	_vertices.push_back(vertex);
}

void Hull::addFace(const glm::vec3 & normal, const std::vector<int>& vertIds) {
	hFace face{ normal, int(_faces.size()) };
	_faces.push_back(face);
	hFace* facePtr = &_faces[_faces.size() - 1];

	/* construct all containing egdes: */
	int p0 = vertIds.size() - 1;
	for (size_t p1 = 0; p1 < vertIds.size(); ++p1) {
		facePtr->vertices.push_back(vertIds[p1]);
		facePtr->edges.push_back(addEdge(facePtr->id, vertIds[p0], vertIds[p1]));
		p0 = p1;
	}

	/* find adjacent faces (brute force): */
	for (size_t i = 0; i < _faces.size() - 1; ++i) {
		hFace& face = _faces[i];
		bool found = false;
		for (size_t j = 0; found == false && j < face.edges.size(); ++j) {
			for (size_t k = 0; found == false && k < facePtr->edges.size(); ++k) {
				if (facePtr->edges[k] == face.edges[j]) {
					facePtr->neighbourFaces.push_back(i);
					face.neighbourFaces.push_back(facePtr->id);
					found = true;
				}
			}
		}
	}

	/* update contained vertices: */
	for (int i = 0; i < vertIds.size(); ++i) {
		hVertex* vStart = &_vertices[_edges[facePtr->edges[i]].vStart];
		hVertex* vEnd = &_vertices[_edges[facePtr->edges[i]].vEnd];

		auto startLoc = std::find(vStart->faces.begin(), vStart->faces.end(), facePtr->id);
		if (startLoc == vStart->faces.end()) {
			vStart->faces.push_back(facePtr->id);
		}

		auto endLoc = std::find(vEnd->faces.begin(), vEnd->faces.end(), facePtr->id);
		if (endLoc == vEnd->faces.end()) {
			vEnd->faces.push_back(facePtr->id);
		}
	}
}

int Hull::edgeID(int v1_ID, int v2_ID) {
	for (const auto& e : _edges) {
		if ((e.vStart == v1_ID && e.vEnd == v2_ID)
			|| (e.vStart == v2_ID && e.vEnd == v1_ID)) 
		{
			return e.id;
		}
	}
	return -1;
}

void Hull::minMaxOnAxis(const glm::vec3 & localAxis, glm::vec3 & outMin, glm::vec3 & outMax) const {
	/* start finding min max values of the vertices projected on our local axis: */
	float projection;
	int minVertex{ 0 }, maxVertex{ 0 };

	float minProjection = FLT_MAX;
	float maxProjection = -FLT_MAX;

	for (size_t i = 0; i < _vertices.size(); ++i) {
		projection = glm::dot(localAxis, _vertices[i].pos);

		if (projection > maxProjection) {
			maxProjection = projection;
			maxVertex = i;
		}
		if (projection < minProjection) {
			minProjection = projection;
			minVertex = i;
		}
	}

	outMin = _vertices[minVertex].pos;
	outMax = _vertices[maxVertex].pos;
}

void Hull::drawDebug(glm::mat4 M) {
	/* draw akk edges: */
	for (const auto& e : _edges) {
		vitiGL::glRendererDebug::instance()->addThickLine(glm::mat3(M) * _vertices[e.vStart].pos, glm::mat3(M) * _vertices[e.vEnd].pos, 0.1f);
	}
}

int Hull::addEdge(int parentFace, int vStart, int vEnd) {
	int id = edgeID(vStart, vEnd);

	/* edge is not registered yet: */
	if (id == -1) {
		id = _edges.size();
		hEdge edge{ vStart, vEnd, id };
		_edges.push_back(edge);
		hEdge* edgePtr = &_edges[id];

		/* find neighbours: */
		for (int i = 0; i < _edges.size(); ++i) {
			if (_edges[i].vStart == vStart || _edges[i].vStart == vEnd
				|| _edges[i].vEnd == vStart || _edges[i].vEnd == vEnd)
			{
				_edges[i].neighbourEdges.push_back(id);
				edgePtr->neighbourEdges.push_back(i);
			}
		}

		/* update contained vertices: */
		_vertices[vStart].edges.push_back(id);
		_vertices[vEnd].edges.push_back(id);
	}

	_edges[id].faces.push_back(parentFace);
	return id;
}
}