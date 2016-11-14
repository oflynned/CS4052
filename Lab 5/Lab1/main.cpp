//Some Windows Headers (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <vector>

#include "Shader.cpp"
#include "Model.cpp"
#include "Light.cpp"
#include "File.cpp"
#include "Camera.cpp"
#include "Player.cpp"
#include "Scene.cpp"
#include "Scenery.cpp"

#include "GLIncludes.h"

#define MESH_NANOSUIT "nanosuit/nanosuit.obj"
#define MESH_ROCK "rock/rock.obj"
#define MESH_SYRINGE "syringe/syringe.obj"
#define MESH_TEAPOT "teapot/teapot.obj"
#define MESH_PLANE "ground/ground.obj"
#define MESH_BOX "box/box.obj"

using namespace std;

Shader shader;
Camera camera;
Light light;
Player player;
Scenery rock, teapot, syringe;
Scene scene;

float last_x = 0.0f, last_y = 0.0f;

GLuint shaderProgramID;
float delta;
int width = 1920;
int height = 1080;

void display() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 view = camera.getView();
	mat4 persp_proj = perspective(camera.getFOV(), (float)width / (float)height, 0.1, 100.0);
	mat4 model = identity_mat4();
	
	shader.use();

	int matrix_location = glGetUniformLocation(shader.getProgram(), "model");
	int view_mat_location = glGetUniformLocation(shader.getProgram(), "view");
	int proj_mat_location = glGetUniformLocation(shader.getProgram(), "projection");
	int light_pos_location = glGetUniformLocation(shader.getProgram(), "lightPos");
	int view_pos_location = glGetUniformLocation(shader.getProgram(), "viewPos");

	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);
	glUniformMatrix4fv(light_pos_location, 1, GL_FALSE, light.getLocation().m);
	glUniformMatrix4fv(view_pos_location, 1, GL_FALSE, camera.getView().m);

	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);
	
	player.draw(shader, matrix_location);
	rock.draw(shader, matrix_location);
	syringe.draw(shader, matrix_location);
	teapot.draw(shader, matrix_location);
	scene.draw(shader, matrix_location);

	glutSwapBuffers();
}

void updateScene() {
	// Placeholder code, if you want to work with framerate
	// Wait until at least 16ms passed since start of last frame (Effectively caps framerate at ~60fps)
	static DWORD  last_time = 0;
	DWORD  curr_time = timeGetTime();
	delta = (curr_time - last_time) * 0.001f;
	if (delta > 0.03f) delta = 0.03f;
	last_time = curr_time;

	scene.update();

	rock.update();
	syringe.update();
	teapot.update();

	camera.update(player);
	player.update(camera.getYaw(), true);

	// Draw the next frame
	glutPostRedisplay();
}

void init() {
	shader = Shader((GLchar*)File::getAbsoluteShaderPath("simpleShader.vert").c_str(),
		(GLchar*)File::getAbsoluteShaderPath("simpleShader.frag").c_str());

	rock = Scenery(vec3(5.0f, -1.0f, 5.0f), MESH_ROCK);
	syringe = Scenery(vec3(5.0f, -1.0f, 0.0f), MESH_SYRINGE);
	teapot = Scenery(vec3(-5.0f, -1.0f, -5.0f), MESH_TEAPOT);

	light = Light(vec3(0.0f, 0.0f, 5.0f), MESH_BOX);
	player = Player(vec3(2.0f, -2.5f, 2.0f), MESH_NANOSUIT);
	scene = Scene(vec3(0.0f, -2.6f, 0.0f), MESH_PLANE);

	Camera camera = Camera(player);
}

// Placeholder code for the keypress
void keypress(unsigned char key, int x, int y) {

	if (key == 27) {
		glutExit();
	}

	player.onKey(key, camera.getTheta());
}

void releaseKeypress(unsigned char key, int x, int y) {
	player.onKeyRelease(key, camera.getTheta());
}

void mouseRoll(int button, int dir, int x, int y) {
	camera.adjustFOV((float) dir);
}

void mouseMove(int x, int y) {

	float xoffset = x - last_x;
	float yoffset = last_y - y;

	last_x = (float) x;
	last_y = (float) y;

	camera.mouseMoveThirdPerson(xoffset, yoffset);
}

int main(int argc, char** argv) {

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Lab 5 - Interactive Walkthrough");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keypress);
	glutKeyboardUpFunc(releaseKeypress);
	glutPassiveMotionFunc(mouseMove);
	glutMouseWheelFunc(mouseRoll);

	//hide cursor for camera
	glutSetCursor(GLUT_CURSOR_NONE);
	//glutFullScreen();

	glShadeModel(GL_SMOOTH);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
	return 0;
}