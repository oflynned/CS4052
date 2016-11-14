#pragma once
#include "GameObject.cpp"

class Syringe : public GameObject {
private:
public:
	Syringe() {}
	Syringe(vec3 pos, std::string meshName, float scale_coeff = 1.0f) : GameObject(pos, meshName, scale_coeff) {

	}
	~Syringe() {}
};