#pragma once
#include "GameObject.cpp"

class Scene : public GameObject {
private:

public:
	Scene() {}
	Scene(vec3 pos, std::string meshName, float scale_coeff = 1.0f) : GameObject(pos, meshName, scale_coeff) {

	}
	~Scene() {}

};