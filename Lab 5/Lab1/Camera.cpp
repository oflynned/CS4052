#pragma once

#include "GLIncludes.h"
#include "Player.cpp"

class Camera {
private:
	const float YAW = 0.0f;
	const float PITCH = 20.0f;
	const float SPEED = 60.0f;
	const float SENSITIVITY = 0.25f;
	const float FOV = 40.0f;

	vec3 pos, front, up, worldUp, right;
	float pitch, yaw, roll;
	float speed, sensitivity, fov;
	float theta;

	// 3rd person attributes where pitch == camera pitch
	Player player;
	float distanceFromPlayer = 10.0f;
	float angleAroundPlayer = 0.0f;
	vec3 playerHeight = vec3(0.0, 2.5f, 0.0f);

	void updateVectors() {
		vec3 front;
		front.v[0] = cos(radians(this->yaw)) * cos(radians(this->pitch));
		front.v[1] = sin(radians(this->pitch));
		front.v[2] = sin(radians(this->yaw)) * cos(radians(this->pitch));

		this->front = normalise(front);
		this->right = normalise(cross(this->front, this->worldUp));
		this->up = normalise(cross(this->right, this->front));
	}

public:
	Camera(vec3 pos = vec3(0.0f, 2.0f, 0.0f)) {
		this->pos = pos;
		this->worldUp = vec3(0.0f, 1.0f, 0.0f);
		this->right = vec3(0.0f, 0.0f, -1.0f);
		this->yaw = YAW;
		this->speed = SPEED;
		this->sensitivity = SENSITIVITY;
		this->fov = FOV;

		updateVectors();
	}

	Camera(const Player& player, vec3 pos = vec3(0.0f, 2.0f, 0.0f)) {
		this->player = player;
		this->pos = pos;
		this->worldUp = vec3(0.0f, 1.0f, 0.0f);
		this->right = vec3(0.0f, 0.0f, -1.0f);
		this->yaw = YAW;
		this->speed = SPEED;
		this->sensitivity = SENSITIVITY;
		this->fov = FOV;
	}

	~Camera() {}

	void update(Player& player) { this->player = player; }

	mat4 getView() { return look_at(pos, (player.getPos() + playerHeight), vec3(0.0f, 1.0f, 0.0f)); }
	float getFOV() { return this->fov; }

	void adjustFOV(float diff) {
		distanceFromPlayer -= diff * 0.3f;
	}
		
	void calcCameraPos(float horizDist, float vertDist) {
		theta = player.getYRot() + angleAroundPlayer;
		float offsetX = horizDist * sin(radians(theta));
		float offsetZ = horizDist * cos(radians(theta));

		pos.v[0] = player.getPos().v[0] - offsetX;
		pos.v[1] = player.getPos().v[1] + vertDist;
		pos.v[2] = player.getPos().v[2] - offsetZ;
	}

	void mouseMoveFirstPerson(float x_offset, float y_offset, bool constrained = true) {
		x_offset *= this->sensitivity;
		y_offset *= this->sensitivity;

		this->yaw += x_offset;
		this->pitch += y_offset;

		if (constrained) {
			// prevent gimbal lock
			if (this->pitch > 89.0f) this->pitch = 89.0f;
			if (this->pitch < -89.0f) this->pitch = -89.0f;
		}

		updateVectors();
	}

	void mouseMoveThirdPerson(float dx, float dy) {
		this->angleAroundPlayer -= dx * 0.3f;
		this->pitch -= dy * 0.1f;

		float horizontalDistance = distanceFromPlayer * cos(radians(pitch));
		float verticalDistance = distanceFromPlayer * sin(radians(pitch));

		calcCameraPos(horizontalDistance, verticalDistance);

		this->yaw = 180 - (player.getYRot() + angleAroundPlayer);
	}

	float getYaw() { return this->yaw; }
	float getTheta() { return this->theta; }
};