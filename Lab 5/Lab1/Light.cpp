#pragma once
#include "GLIncludes.h"
#include "GameObject.cpp"

class Light : public GameObject {
private:
	bool isVisible = true;
public:
	Light() {}
	Light(vec3 pos, std::string meshName, float scale_coeff = 1.0f) 
		: GameObject(pos, meshName, scale_coeff) {

	}
	
	~Light() {}

	void setVisibility(bool isVisible) { this->isVisible = isVisible; }
	bool getVisibility() { return this->isVisible; }

	mat4 getLocation() {
		mat4 location = identity_mat4();
		location = translate(location, getPos());
		return location;
	}
};

