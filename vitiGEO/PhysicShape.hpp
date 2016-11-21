#pragma once

#include <vector>
#include <list>
#include <glm/glm.hpp>

#include "Hull.hpp"
#include "Geometry.hpp"

namespace vitiGEO {

class PhysicObject;

/*	---------------------------------------------------------------------------
	Abstract interface for different kind of shapes: 
	-------------------------------------------------------------------------- */

class IPhysicShape {
public:
	IPhysicShape(PhysicObject* owner) 
		: _owner { owner }
	{};

	virtual ~IPhysicShape() {};

	virtual glm::mat3 inverseInertia(float invMass) const = 0;

	/* return the oriented normals of the shape: */
	virtual void collisionNormals(std::vector<glm::vec3>& axes) const = 0;
	virtual void edges(std::vector<glm::vec3>& edges) const = 0;

	/* returns the minimum and maximum vertex along a given axis: */
	virtual void minMaxOnAxis(const glm::vec3& axis, glm::vec3& outMin, glm::vec3& outMax) const = 0;

	/* computes the face that is closest to parallel to a given axis, returns the face, face normal, 
	   and the planes of adjacent faces: */
	virtual void calcIncident(const glm::vec3& axis,
							std::list<glm::vec3>& outFace,
							glm::vec3& outNormal,
							std::vector<Plane>& outAdjacentPlanes) = 0;

	virtual void debugDraw() = 0;

protected:
	PhysicObject* _owner;
};

/*	---------------------------------------------------------------------------- 
	Cuboid Shape : 
	---------------------------------------------------------------------------- */
	
class CuboidShape : public IPhysicShape {
public:
	CuboidShape(PhysicObject* owner, const glm::vec3& halfSize = { 0.5f, 0.5f, 0.5f });
	~CuboidShape();

	virtual glm::mat3 inverseInertia(float invMass) const override;

	virtual void collisionNormals(std::vector<glm::vec3>& axes) const override;
	virtual void edges(std::vector<glm::vec3>& edges) const override;
	virtual void minMaxOnAxis(const glm::vec3 & axis, glm::vec3 & outMin, glm::vec3 & outMax) const override;

	virtual void calcReferenceFace(const glm::vec3& axis,
							std::list<glm::vec3>& outFace,
							glm::vec3& outNormal,
							std::vector<Plane>& outAdjacentPlanes);

	void setHalfSize(const glm::vec3& half) { _halfSize = half; }
	glm::vec3 halfSize() const { return _halfSize; }

	virtual void debugDraw() override;

protected:
	static void initHull();

	static Hull _hull;
	glm::vec3	_halfSize;
};

}

