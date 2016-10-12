#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

#include "Math.h"

class Transform {
private:
	GLfloat rotate_x, rotate_y, rotate_z;
	GLfloat x, y, z;
	GLfloat posVar, scaleVar;
	GLfloat temp_x, temp_y, temp_z;
	
	mat4 transformation = identity_mat4();
	mat4 eye = identity_mat4();

public:
	Transform() {
		this->scaleVar = 0.0f;
		this->posVar = this->x = this->y = this->z = 0;
	}

	void uniformScale(GLfloat scale) {
		transformation.m[0] += scale;
		transformation.m[5] += scale;
		transformation.m[10] += scale;
	}

	void nonUniformScale(GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ) {
		transformation.m[0] += scaleX;
		transformation.m[5] += scaleY;
		transformation.m[10] += scaleZ;
	}

	void translateOrigin() {
		this->temp_x = this->x;
		this->temp_y= this->y;
		this->temp_z = this->z;

		transformation.m[3] = 0;
		transformation.m[7] = 0;
		transformation.m[11] = 0;
	}

	void resetTranslation() {
		transformation.m[3] = this->temp_x;
		transformation.m[7] = this->temp_y;
		transformation.m[11] = this->temp_z;
	}

	void translateToPos(GLfloat x, GLfloat y, GLfloat z) {
		transformation.m[3] = x;
		transformation.m[7] = y;
		transformation.m[11] = z;
	}

	void translate(GLfloat x, GLfloat y, GLfloat z) {
		transformation.m[3] += x;
		transformation.m[7] += y;
		transformation.m[11] += z;
	}

	void rotateX(GLfloat angle) {
		rotate_x += angle;
		transformation.m[5] = cos(rotate_x);
		transformation.m[6] = sin(rotate_x) * -1;
		transformation.m[9] = sin(rotate_x);
		transformation.m[10] = cos(rotate_x);
	}

	void rotateY(GLfloat angle) {
		rotate_y += angle;
		transformation.m[0] = cos(rotate_y);
		transformation.m[2] = sin(rotate_y);
		transformation.m[8] = sin(rotate_y) * -1;
		transformation.m[10] = cos(rotate_y);
	}

	void rotateZ(GLfloat angle) {
		rotate_z += angle;
		transformation.m[0] = cos(rotate_z);
		transformation.m[1] = sin(rotate_z) * -1;
		transformation.m[4] = sin(rotate_z);
		transformation.m[5] = cos(rotate_z);
	}

	void reset() {
		rotate_x = rotate_y = rotate_z = posVar = scaleVar = 0.0f;
		transformation = identity_mat4();
	}

	void combinedTransformations() {
		rotate_x += 0.01f;
		scaleVar += 0.01f;
		posVar += 0.01f;

		mat4 translationTransformation = identity_mat4();
		translationTransformation.m[3] += posVar;
		translationTransformation.m[7] += posVar;
		translationTransformation.m[11] += posVar;

		mat4 scaledTransformation = identity_mat4();
		scaledTransformation.m[0] -= scaleVar;
		scaledTransformation.m[5] -= scaleVar;
		scaledTransformation.m[10] -= scaleVar;

		mat4 rotationTransformation = identity_mat4();
		rotationTransformation.m[5] = cos(rotate_x);
		rotationTransformation.m[6] = sin(rotate_x) * -1;
		rotationTransformation.m[9] = sin(rotate_x);
		rotationTransformation.m[10] = cos(rotate_x);

		transformation = translationTransformation * scaledTransformation * rotationTransformation;
	}

	mat4 getMatrix() { return this->transformation; }
};