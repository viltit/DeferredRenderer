/* Class for representing 3D-Models loaded from an obj File */

#pragma once

#include <string>
#include "Shape.hpp"

namespace vitiGL {

class Mesh : public Shape {
public:
	Mesh(const std::string& filePath);
	~Mesh();

private:
	void initVertices();

	// Inherited via Shape
	virtual void initVertices(std::vector<Vertex>& vertices) override;
};

}
