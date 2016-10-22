#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

#include "maths_funcs.h" //Anton's math class

class Perspective
{
public:
	enum Rotation {
		x, y, z, none
	};

	enum Position
	{
		tl, tr, bl, br
	};

private:
	mat4 view, proj, model;
	float angle;
	Position position;

public:
	Perspective() {}

	Perspective(Position position, float fov, vec3 pos, Rotation rotation = Rotation::none, float angle = 0) {
		this->position = position;
		this->angle = angle;
		this->view = translate(identity_mat4(), pos);
		this->proj = perspective(fov, (float)width / (float)height, 0.1, 100);
		switch (rotation) {
		case Perspective::x:
			model = rotate_x_deg(identity_mat4(), angle);
			break;
		case Perspective::y:
			model = rotate_y_deg(identity_mat4(), angle);
			break;
		case Perspective::z:
			model = rotate_z_deg(identity_mat4(), angle);
			break;
		default:
			model = identity_mat4();
			break;
		}
	}

	Perspective(Position position, mat4 ortho, float fov, Rotation rotation = Rotation::none, float angle = 0) {
		this->position = position;
		this->angle = angle;
		this->view = look_at(vec3(0.0f, 0.0f, 35.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		this->proj = ortho; // perspective(fov, (float)width / (float)height, 0.1, 100);
		switch (rotation) {
		case Perspective::x:
			model = rotate_x_deg(identity_mat4(), angle);
			break;
		case Perspective::y:
			model = rotate_y_deg(identity_mat4(), angle);
			break;
		case Perspective::z:
			model = rotate_z_deg(identity_mat4(), angle);
			break;
		default:
			model = identity_mat4();
			break;
		}
	}

	~Perspective() {}

	float width = 800.0f;
	float height = 600.0f;

	mat4 getView() { return this->view; }
	mat4 getProj() { return this->proj; }
	mat4 getModel() { return this->model; }

	float getAngle() { return this->angle; }
	void rotateObject(float angle) {
		this->angle += angle;
		model = rotate_x_deg(identity_mat4(), this->angle) *
			rotate_y_deg(identity_mat4(), this->angle) *
			rotate_z_deg(identity_mat4(), this->angle);
	}

	void spin(float angle) {
		this->angle += angle;
		model = rotate_y_deg(identity_mat4(), this->angle);
	}

	void draw(int proj_mat_location, int view_mat_location, int matrix_location, int teapot_vertex_count) {
		switch (position) {
		case tl:
			glViewport(0, height / 2, width / 2, height / 2);
			break;
		case tr:
			glViewport(width / 2, height / 2, width / 2, height / 2);
			break;
		case bl:
			glViewport(0, 0, width / 2, height / 2);
			break;
		case br:
			glViewport(width / 2, 0, width / 2, height / 2);
			break;
		}

		glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, getProj().m);
		glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, getView().m);
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, getModel().m);
		glDrawArrays(GL_TRIANGLES, 0, teapot_vertex_count);
	}

	static mat4 ortho(float b, float t, float l, float r, float n, float f) {
		mat4 m = zero_mat4();
		m.m[0] = (2 / (r - l));
		m.m[3] = -((r + l) / (r - l));
		m.m[5] = (2 / (t - b));
		m.m[7] = -((t + b) / (t - b));
		m.m[10] = -(2 / (f - n));
		m.m[11] = -((f + n) / (f - n));
		m.m[15] = 1.0f;
		return m;
	}
};