#pragma once
#include "GameObject.cpp"

class Scenery : public GameObject {
private:
public:
	Scenery() {}
	Scenery(vec3 pos, std::string meshName, float scale_coeff = 1.0f) : GameObject(pos, meshName, scale_coeff) {

	}
	~Scenery() {}
};