#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace vitiGEO {

class PhysicObject;

class PhysicShape {
public:
	PhysicShape(PhysicObject* owner, const std::vector<glm::vec3>& vertices);
	~PhysicShape();

	void normals(std::vector<glm::vec3>& axes);

protected:
	std::vector<glm::vec3> _vertices;
	std::vector<glm::vec3> _normals;
	std::vector<glm::vec3> _edges;

	PhysicObject* _owner;
};

}

