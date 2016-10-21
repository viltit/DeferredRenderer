#include "AABB.hpp"
#include "Math.hpp"

#include <limits>
#include <cmath>
#include <assert.h>

namespace vitiGEO {

AABB::AABB()  {
	float minF = std::numeric_limits<float>::min();
	float maxF = std::numeric_limits<float>::max();

	_min = { maxF, maxF, maxF };
	_max = { minF, minF, minF };
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


	for (const auto& point : vertices) {
		addPoint(point);
	}

	_oMin = _min;
	_oMax = _max;
}

/* adapt the bounding box to a transformed object */
void AABB::transform(const glm::mat4 & M) {
	if (isEmpty()) return;
	_min = _max = getTranslation(M);

	/* matrix first column (glm::mat4 has column-row reversed !) */
	if (M[0][0] > 0.0f) {
		_min.x += M[0][0] * _oMin.x;
		_max.x += M[0][0] * _oMax.x;
	}
	else {
		_min.x += M[0][0] * _oMax.x;
		_max.x += M[0][0] * _oMin.x;
	}
	if (M[0][1] > 0.0f) {
		_min.y += M[0][1] * _oMin.x;
		_max.y += M[0][1] * _oMax.x;
	}
	else {
		_min.y += M[0][1] * _oMax.x;
		_max.y += M[0][1] * _oMin.x;
	}
	if (M[0][2] > 0.0f) {
		_min.z += M[0][2] * _oMin.x;
		_max.z += M[0][2] * _oMax.x;
	}
	else {
		_min.z += M[0][2] * _oMax.x;
		_max.z += M[0][2] * _oMin.x;
	}

	/* second column */
	if (M[1][0] > 0.0f) {
		_min.x += M[1][0] * _oMin.y;
		_max.x += M[1][0] * _oMax.y;
	}
	else {
		_min.x += M[1][0] * _oMax.y;
		_max.x += M[1][0] * _oMin.y;
	}
	if (M[1][1] > 0.0f) {
		_min.y += M[1][1] * _oMin.y;
		_max.y += M[1][1] * _oMax.y;
	}
	else {
		_min.y += M[1][1] * _oMax.y;
		_max.y += M[1][1] * _oMin.y;
	}
	if (M[1][2] > 0.0f) {
		_min.z += M[1][2] * _oMin.y;
		_max.z += M[1][2] * _oMax.y;
	}
	else {
		_min.z += M[1][2] * _oMax.y;
		_max.z += M[1][2] * _oMin.y;
	}

	/* third column */
	if (M[2][0] > 0.0f) {
		_min.x += M[2][0] * _oMin.z;
		_max.x += M[2][0] * _oMax.z;
	}
	else {
		_min.x += M[2][0] * _oMax.z;
		_max.x += M[2][0] * _oMin.z;
	}
	if (M[2][1] > 0.0f) {
		_min.y += M[2][1] * _oMin.z;
		_max.y += M[2][1] * _oMax.z;
	}
	else {
		_min.y += M[2][1] * _oMax.z;
		_max.y += M[2][1] * _oMin.z;
	}
	if (M[2][2] > 0.0f) {
		_min.z += M[2][2] * _oMin.z;
		_max.z += M[2][2] * _oMax.z;
	}
	else {
		_min.z += M[2][2] * _oMax.z;
		_max.z += M[2][2] * _oMin.z;
	}
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

glm::vec3 AABB::getClosestPoint (const glm::vec3 & p) const {
	glm::vec3 r{};

	if (p.x < _min.x) r.x = _min.x;
	else if (p.x > _max.x) r.x = _max.x;
	else r.x = p.x;

	if (p.y < _min.y) r.y = _min.y;
	else if (p.y > _max.y) r.y = _max.y;
	else r.y = p.y;

	if (p.z < _min.z) r.z = _min.z;
	else if (p.z > _max.z) r.z = _max.z;
	else r.z = p.z;

	return r;
}

void AABB::addPoint(const glm::vec3 & p) {
	if (p.x < _min.x) _min.x = p.x;
	if (p.x > _max.x) _max.x = p.x;
	if (p.y < _min.y) _min.y = p.y;
	if (p.y > _max.y) _max.y = p.y;
	if (p.z < _min.z) _min.z = p.z;
	if (p.z > _max.z) _max.z = p.z;
}


bool AABBIntersection(const AABB & box1, const AABB & box2, AABB * intersect) {
	/* check for overlap: */
	glm::vec3 min1 = box1.min();
	glm::vec3 min2 = box2.min();
	glm::vec3 max1 = box1.max();
	glm::vec3 max2 = box2.max();

	if (min1.x > max2.x) return false;
	if (max1.x < min2.x) return false;
	if (min1.y > max2.y) return false;
	if (max1.y < min2.y) return false;
	if (min1.z > max2.z) return false;
	if (max1.z < min2.z) return false;

	/* we have an overlap. Return true or calculate the intersection box: */
	if (intersect == nullptr) return true;

	glm::vec3 mini;
	glm::vec3 maxi;

	mini.x = fmax(min1.x, min2.x);
	maxi.x = fmin(max1.x, max2.x);
	mini.y = fmax(min1.y, min2.y);
	maxi.y = fmin(max1.y, max2.y);
	mini.z = fmax(min1.z, min2.z);
	maxi.z = fmin(max1.z, max2.z);

	intersect->setMin(mini);
	intersect->setMax(maxi);

	return true;
}
}