#include "Collidor.hpp"

#include "PhysicObject.hpp"
#include "PhysicShape.hpp"
#include "AABB.hpp"
#include "Manifold.hpp"

namespace vitiGEO {

Collidor::Collidor(){
}

Collidor::~Collidor() {
}

bool Collidor::AABBIntersect(const PhysicObject * obj1, const PhysicObject * obj2) {
	if (AABBIntersection(obj1->aabb(), obj2->aabb())) {
		//return true;
		return false;
	}
	return false;
}

bool Collidor::SAT(const PhysicObject * obj1, const PhysicObject * obj2, CollidorData & out) {
	CollidorData currentCollision;
	CollidorData bestCollision;
	bestCollision.depth = -FLT_MAX;

	/* Assemble the axes we need to test against: */
	std::vector<glm::vec3> axes;
	obj1->shape()->collisionNormals(axes);
	obj2->shape()->collisionNormals(axes);

	/* Add edge-edge axes */
	std::vector<glm::vec3> edges1;
	std::vector<glm::vec3> edges2;
	obj1->shape()->edges(edges1);
	obj2->shape()->edges(edges2);

	for (auto& E1 : edges1) {
		for (auto& E2 : edges2) {
			E1 = glm::normalize(E1);
			E2 = glm::normalize(E2);
			addSATAxis(glm::cross(E1, E2), axes);
		}
	}

	/* test if we find a separating axis between the two objs and bail out if we do: */
	for (const auto& a : axes) {
		if (!testSATAxis(a, obj1, obj2, currentCollision)) {
			return false;
		}
		if (currentCollision.depth >= bestCollision.depth) {
			bestCollision = currentCollision;
		}
	}   
	out = bestCollision;
	
	//std::cout << "Depth: " << out.depth << std::endl;
	//std::cout << "Hit normal: " << out.hitNormal.x << "/" << out.hitNormal.y << "/" << out.hitNormal.z << std::endl;
	return true;
}

/* naming:	- the incident face is the one we clip to find the collision contact point
			- the reference face is the one we clip against */
void Collidor::buildManifold(const PhysicObject * obj1, const PhysicObject * obj2, 
	const CollidorData & collision, Manifold * manifold) 
{
	assert(manifold);

	/* prepare variables: */
	std::list<glm::vec3> polygon1;
	std::list<glm::vec3> polygon2;
	glm::vec3 normal1;
	glm::vec3 normal2;
	std::vector<Plane> adjacentPlanes1;
	std::vector<Plane> adjacentPlanes2;

	/* get the data to calculate reference and incident polygon: */
	obj1->shape()->calcIncidentReference(collision.hitNormal, polygon1, normal1, adjacentPlanes1);
	obj2->shape()->calcIncidentReference(-collision.hitNormal, polygon2, normal2, adjacentPlanes2);

	if (polygon1.size() == 0 || polygon2.size() == 0) return;

	if (polygon1.size() == 1);
	if (polygon2.size() == 1); //we have a curve -> not supported yet

	/* find out which face we clip against the other: */
	bool flipped{ false };
	std::list<glm::vec3>* incPoly{ nullptr }; //pointer to incident Polygon which will be clipped
	glm::vec3* incNormal{ nullptr };
	std::vector<Plane>* refAdjPlanes { nullptr };
	Plane refPlane;

	/* which polygon's normal is more parallel to the collision normal? */
	if (fabs(glm::dot(collision.hitNormal, normal1)) >
		fabs(glm::dot(collision.hitNormal, normal2))) 
	{
		float planeDistance = -glm::dot(-normal1, polygon1.front());
		refPlane.setNormal(-normal1);
		refPlane.setDistance(planeDistance);
		refAdjPlanes = &adjacentPlanes1;

		incPoly = &polygon2;
		incNormal = &normal2;

		flipped = false;
	}
	else {
		float planeDistance = -glm::dot(-normal2, polygon2.front());
		refPlane.setNormal(-normal2);
		refPlane.setDistance(planeDistance);
		refAdjPlanes = &adjacentPlanes2;

		incPoly = &polygon1;
		incNormal = &normal1;

		flipped = true;
	}

	/* Clip the incident face to the adjacent edges of the reference face */
	SutherlandHodgesonClipping(*incPoly, refAdjPlanes->size(), refAdjPlanes->data(), *incPoly, false);
	
	/* clip and remove any contact point that are above the reference face: */
	SutherlandHodgesonClipping(*incPoly, 1, &refPlane, *incPoly, true);

	/* we can now finally build the contact manifold: */
	glm::vec3 startPoint = incPoly->back();
	for (const auto& endPoint : *incPoly) {
		float depth{ 0.0f };
		glm::vec3 globalOnA;
		glm::vec3 globalOnB;

		if (flipped) {
			depth = -(glm::dot(endPoint, collision.hitNormal)) - glm::dot(collision.hitNormal, polygon2.front());
			globalOnA = endPoint + collision.hitNormal * depth;
			globalOnB = endPoint;
		}
		else {
			depth = glm::dot(endPoint, collision.hitNormal) - glm::dot(collision.hitNormal, polygon1.front());
			globalOnA = endPoint;
			globalOnB = endPoint - collision.hitNormal * depth;
 		}

		manifold->addContact(globalOnA, globalOnB, collision.hitNormal, depth);
		startPoint = endPoint;
	}
}

void Collidor::addSATAxis(const glm::vec3& axis, std::vector<glm::vec3>& SATAxes) {
	if (glm::dot(axis, axis) < EPSILON_TRESHOLD) return;

	for (const auto& a : SATAxes) {
		if (glm::dot(a, axis) > 1.0f - EPSILON_TRESHOLD) return;
	}

	SATAxes.push_back(axis);
}
	
bool Collidor::testSATAxis(const glm::vec3 axis, const PhysicObject * obj1, const PhysicObject * obj2, CollidorData & out) const {
	glm::vec3 min1, min2, max1, max2;

	obj1->shape()->minMaxOnAxis(axis, min1, max1);
	obj2->shape()->minMaxOnAxis(axis, min2, max2);

	float minProjection1 = glm::dot(axis, min1);
	float maxProjection1 = glm::dot(axis, max1);
	float minProjection2 = glm::dot(axis, min2);
	float maxProjection2 = glm::dot(axis, max2);

	if (minProjection1 <= minProjection2 && maxProjection1 >= minProjection2) {
		out.hitNormal = axis;
		out.depth = minProjection2 - maxProjection1;
		out.hitPoint = max1 + out.hitNormal * out.depth;
		return true;
	}

	if (minProjection2 <= minProjection1 && maxProjection2 > minProjection1) {
		out.hitNormal = -axis;
		out.depth = minProjection1 - maxProjection2;
		out.hitPoint = min1 + out.hitNormal * out.depth;
		return true;
	}

	return false;
}

void Collidor::SutherlandHodgesonClipping(const std::list<glm::vec3>& inPoly, int numClippingPlanes, 
	const Plane* clippingPlanes, std::list<glm::vec3>& outPoly, bool removePoints)
{
	std::list<glm::vec3> tPoly1;
	std::list<glm::vec3> tPoly2;
	std::list<glm::vec3>* input = &tPoly1;
	std::list<glm::vec3>* output = &tPoly2;

	*output = inPoly;
	for (size_t i = 0; i < numClippingPlanes; ++i) {
		if (output->empty()) break;

		const Plane& plane = clippingPlanes[i];
		std::swap(input, output);
		output->clear();

		glm::vec3 startPoint = input->back();
		for (const auto& endPoint : *input) {
			bool isStartInPlane = plane.pointInPlane(startPoint);
			bool isEndInPlane = plane.pointInPlane(endPoint);

			if (removePoints) {
				if (isEndInPlane) output->push_back(endPoint);
			}
			else {
				if (isStartInPlane && isEndInPlane)
					output->push_back(endPoint);
				else if (isStartInPlane && !isEndInPlane)
					output->push_back(plane.planeEdgeIntersection(startPoint, endPoint));
				else if (!isStartInPlane && isEndInPlane) {
					output->push_back(plane.planeEdgeIntersection(endPoint, startPoint));
					output->push_back(endPoint);
				}
			}

			startPoint = endPoint;
		}
	}
	outPoly = *output; //may be wrong
}

}