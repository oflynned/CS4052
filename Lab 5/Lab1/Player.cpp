#pragma once
#include "GameObject.cpp"

class Player : public GameObject {
public:
	enum State {
		MOVING, IDLE, INTERACTING
	};

	Player() {}
	Player(vec3 pos, std::string meshName, float scale_coeff = 0.2f) : GameObject(pos, meshName, scale_coeff) {

	}
	~Player() {}

	void update(float cameraRot = 0.0f, bool hasCamera = false, bool isFlip = false) override {
		modelMat = identity_mat4();
		modelMat = rotate_x_deg(modelMat, rot_x);

		modelMat = state == State::MOVING ? 
			rotate_y_deg(modelMat, 180 - cameraRot) :
			rotate_y_deg(modelMat, rot_y);

		modelMat = rotate_z_deg(modelMat, rot_z);
		modelMat = scale(modelMat, scale_tuple);
		modelMat = translate(modelMat, pos);
	}

	void onKey(unsigned char key, float cameraRot) override {
		switch (key) {
		case 'w':
			setState(State::MOVING);
			translateObject(vec3(0.5f * sin(radians(cameraRot)), 0.0f, 0.5f * cos(radians(cameraRot))));
			break;
		case 's':
			setState(State::MOVING);
			translateObject(vec3(-0.5f * sin(radians(cameraRot)), 0.0f, -0.5f * cos(radians(cameraRot))));
			break;
		case 'a':
			setState(State::IDLE);
			rotate(-1.0f);
			break;
		case 'd':
			setState(State::IDLE);
			rotate(1.0f);
			break;
		case 'e':
			setState(State::INTERACTING);
			break;
		default:
			setState(State::IDLE);
			break;
		}
	}

	void onKeyRelease(unsigned char key, float cameraRot) override {
		setState(State::IDLE);
		setYRot(cameraRot);
	}

	void setState(State state) { this->state = state; }
	State getState() { return this->state; }

private:
	State state;
};