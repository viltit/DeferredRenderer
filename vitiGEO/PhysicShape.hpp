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

/*	---------------------------------------------------------------------------
	Abstract interface for different kind of shapes: 
	-------------------------------------------------------------------------- */

class IPhysicShape {
public:
	IPhysicShape(PhysicObject* owner) 
		: _owner { owner }
	{};

	virtual ~IPhysicShape() {};

	/* return the oriented normals of the shape: */
	virtual void normals(std::vector<glm::vec3>& axes) = 0;
	virtual void edges(std::vector<glm::vec3>& edges) = 0;

	virtual void minMaxOnAxis(const glm::vec3& axis, glm::vec3& outMin, glm::vec3& outMax) const = 0;

	glm::vec3& vertex(int index) { return _vertices[index]; }

protected:
	std::vector<glm::vec3> _vertices;
	std::vector<glm::vec3> _normals;
	std::vector<Edge> _edges;

	PhysicObject* _owner;
};

/*	Cuboid Shape : -------------------------------------------------------------- */
class CuboidShape : public IPhysicShape {
public:
	CuboidShape(PhysicObject* owner);

	virtual void normals(std::vector<glm::vec3>& axes) override;
	virtual void edges(std::vector<glm::vec3>& edges) override;
	virtual void minMaxOnAxis(const glm::vec3 & axis, glm::vec3 & outMin, glm::vec3 & outMax) const override;

protected:
	
};

}

