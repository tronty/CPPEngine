#pragma once
#include "LSystem.h"
#include "Model.h"

class Model;
class MeshGenerator {
public:
	LSytem m_system;
	Model* m_model;
	float rotationOffset = D3DXFROMWINE_PI / 8;
	float scaleOffset = 1.5f;
	float translationOffset = 1.0f;
	void GenerateModel(std::string system, int iterations, std::string modelName, D3DXFROMWINEVECTOR3 startingPoint, float radius, int pointsPerLevel);

private:

};
