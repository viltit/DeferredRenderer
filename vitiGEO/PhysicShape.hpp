#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace vitiGEO {

class PhysicObject;

class PhysicShape {
public:
	PhysicShape(PhysicObject* owner, const std::vector<glm::vec3>& vertices);
	~PhysicShape();

	/* return the oriented normals of the shape: */
	void normals(std::vector<glm::vec3>& axes);

	void minMaxOnAxis(const glm::vec3& axis, glm::vec3& outMin, glm::vec3& outMax) const;

protected:
	std::vector<glm::vec3> _vertices;
	std::vector<glm::vec3> _normals;
	std::vector<glm::vec3> _edges;

	PhysicObject* _owner;
};

}

