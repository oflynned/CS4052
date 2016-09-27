#include <GL/glew.h>
#include <GL/freeglut.h>

#pragma once

class Triangle
{
private:
	GLfloat vertices[];
	GLfloat colours[];

public:
	Triangle();
	Triangle(GLfloat vertices[], GLfloat colours[]);
	~Triangle();

	GLfloat getVertices() {
		return this->vertices;
	}

	GLfloat getColours() {
		return this->colours;
	}
};

