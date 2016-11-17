#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace vitiGEO {

class PhysicObject;

struct Edge {
	Edge(int begin, int end)
		:	eBegin{ begin },
			eEnd { end }
	{}

	int eBegin;
	int eEnd;
};

class PhysicShape {
public:
	PhysicShape(PhysicObject* owner, const std::vector<glm::vec3>& vertices);
	~PhysicShape();

	/* return the oriented normals of the shape: */
	void normals(std::vector<glm::vec3>& axes);
	void edges(std::vector<glm::vec3>& edges);

	glm::vec3& vertex(int index)  { return _vertices[index]; }

	void minMaxOnAxis(const glm::vec3& axis, glm::vec3& outMin, glm::vec3& outMax) const;

protected:
	std::vector<glm::vec3> _vertices;
	std::vector<glm::vec3> _normals;
	std::vector<Edge> _edges;

	PhysicObject* _owner;
};

}

