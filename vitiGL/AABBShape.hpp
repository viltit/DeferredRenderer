/* Class for visualizing an objects AABB */

#pragma once

#include <AABB.hpp>

#include "Shape.hpp"
#include "Shader.hpp"

namespace vitiGL {

class Shader;
class Camera;

class AABBShape : public ShapeI {
public:
	AABBShape(const vitiGEO::AABB* box, Camera* cam);
	AABBShape(const AABBShape&);
	~AABBShape();

	/*	We deviate from our normal rules here: The AABB is using its own shader 
		(so if another shader is on when calling this function, it will be off afterward) */
	void draw(const Shader& shader) const override { draw(); };
	void draw() const;

	/* for debug purpose only: */
	void update(vitiGEO::AABB* box);

private:
	virtual void initVertices(std::vector<Vertex>& vertices) override;

	const vitiGEO::AABB* _box;
	Camera* _cam;

	Shader _shader;
};
}
