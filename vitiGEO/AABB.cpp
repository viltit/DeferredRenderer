#include "AABB.hpp"
#include "Math.hpp"

#include <limits>
#include <assert.h>

namespace vitiGEO {

AABB::AABB() {
}

AABB::AABB(std::vector<glm::vec3>& vertices) {
	construct(vertices);
}

AABB::~AABB() {
}

void AABB::construct(std::vector<glm::vec3>& vertices) {
	float minF = std::numeric_limits<float>::min();
	float maxF = std::numeric_limits<float>::max();

	_min = { maxF, maxF, maxF };
	_max = { minF, minF, minF };

	for (int i = 0; i < vertices.size(); i++)
		vertices[i] *= 0.05f;

	for (const auto& point : vertices) {
		addPoint(point);
	}
}

/* adapt the bounding box to a transformed object */
void AABB::transform(const glm::mat4 & M) {
	if (isEmpty()) return;

	glm::vec3 temp{ oldT };
	glm::vec3 t = getTranslation(M);
	oldT = t;
	t -= temp;

	_min += t;
	_max += t;
}

/* Corners: 
	i = 0: top right front
	i = 1: top left front
	i = 2: bottom right front
	i = 3: top right front
	i = 4: top right back
	i = 5: top left back
	i = 6: bottom right back
	i = 7: bottom left back
*/
glm::vec3 AABB::corner(int i) const {
	assert(i >= 0 && i <= 7);
	return glm::vec3 {
		(i & 1) ? _min.x : _max.x,
		(i & 2) ? _min.y : _max.y,
		(i & 4) ? _min.z : _max.z
	};
}

void AABB::addPoint(const glm::vec3 & p) {
	if (p.x < _min.x) _min.x = p.x;
	if (p.x > _max.x) _max.x = p.x;
	if (p.y < _min.y) _min.y = p.y;
	if (p.y > _max.y) _max.y = p.y;
	if (p.z < _min.z) _min.z = p.z;
	if (p.z > _max.z) _max.z = p.z;
}
}