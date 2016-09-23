/* Class for representing 3D-Models loaded from an obj File */

#pragma once

#include <string>
#include <vector>
#include "IGameObject.hpp"

namespace vitiGL {

class Mesh;

class Model : public IGameObject {
public:
	Model(const std::string& filePath);
	~Model();

	// Inherited via IGameObject
	virtual void setModelMatrix(const glm::mat4& M) override;

	virtual void draw(const Shader & shader) const override;
	virtual void drawNaked(const Shader& shader) const override;

private:
	std::vector<Mesh> _mesh;
};

}