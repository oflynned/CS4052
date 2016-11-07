#pragma warning(disable:4996)

//Some Windows Headers (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include "maths_funcs.h"
#include "teapot.h" // teapot mesh

// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace std;
GLuint shaderProgramID;

unsigned int teapot_vao = 0;
int width = 800;
int height = 600;

GLuint loc1;
GLuint loc2;
GLfloat rotatez_tier_1 = 0.0f;
GLfloat rotatez_tier_2 = 0.0f;

mat4 view;
mat4 persp_proj;
mat4 base;

static const float TEAPOT_HEIGHT = 15.0f;

// Shader Functions- click on + to expand
#pragma region SHADER_FUNCTIONS

// Create a NULL-terminated string by reading the provided file
char* readShaderSource(const char* shaderFile) {
	FILE* fp = fopen(shaderFile, "rb"); //!->Why does binary flag "RB" work and not "R"... wierd msvc thing?

	if (fp == NULL) { return NULL; }

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	buf[size] = '\0';

	fclose(fp);

	return buf;
}


static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}
	const char* pShaderSource = readShaderSource(pShaderText);

	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	// compile the shader and check for errors
	glCompileShader(ShaderObj);
	GLint success;
	// check for shader related errors using glGetShaderiv
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		exit(1);
	}
	// Attach the compiled shader object to the program object
	glAttachShader(ShaderProgram, ShaderObj);
}

GLuint CompileShaders()
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
	shaderProgramID = glCreateProgram();
	if (shaderProgramID == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	// Create two shader objects, one for the vertex, and one for the fragment shader
	AddShader(shaderProgramID, "../Lab1/Shaders/simpleVertexShader.txt", GL_VERTEX_SHADER);
	AddShader(shaderProgramID, "../Lab1/Shaders/simpleFragmentShader.txt", GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };
	// After compiling all shader objects and attaching them to the program, we can finally link it
	glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
	glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
	glUseProgram(shaderProgramID);
	return shaderProgramID;
}
#pragma endregion SHADER_FUNCTIONS

// VBO Functions - click on + to expand
#pragma region VBO_FUNCTIONS

void generateObjectBufferTeapot() {
	GLuint vp_vbo = 0;

	loc1 = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2 = glGetAttribLocation(shaderProgramID, "vertex_normals");

	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * teapot_vertex_count * sizeof(float), teapot_vertex_points, GL_STATIC_DRAW);
	GLuint vn_vbo = 0;
	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * teapot_vertex_count * sizeof(float), teapot_normals, GL_STATIC_DRAW);

	glGenVertexArrays(1, &teapot_vao);
	glBindVertexArray(teapot_vao);

	glEnableVertexAttribArray(loc1);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}


#pragma endregion VBO_FUNCTIONS

// calculates displacement from previous level (difference should be 1)
float getTierDisplacement(int level) {
	return -TEAPOT_HEIGHT * level;
}

void display() {

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	//Declare your uniform variables that will be used in your shader
	int matrix_location = glGetUniformLocation(shaderProgramID, "model");
	int view_mat_location = glGetUniformLocation(shaderProgramID, "view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID, "proj");

	// Hierarchy of Teapots

	// 1st teapot
	// Root of the Hierarchy
	// for the root, we orient it in global space
	mat4 root = base;
	// update uniforms & draw
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, root.m);
	glDrawArrays(GL_TRIANGLES, 0, teapot_vertex_count);

	// 2nd teapot
	// child of hierarchy
	mat4 local2_0 = identity_mat4();
	local2_0 = rotate_y_deg(local2_0, rotatez_tier_1) 
		* translate(local2_0, vec3(getTierDisplacement(1) / 4, -10, 0.0)) 
		* scale(local2_0, vec3(0.5f, 0.5f, 0.5f));
	mat4 global2 = root*local2_0;
	// update uniform & draw
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, global2.m);
	glDrawArrays(GL_TRIANGLES, 0, teapot_vertex_count);

	//3rd teapot
	//2nd tier
	mat4 local3_0 = identity_mat4();
	local3_0 = rotate_y_deg(local3_0, rotatez_tier_2) 
		* translate(local3_0, vec3(0.0, getTierDisplacement(1), 0.0));
	mat4 global3 = root*local2_0*local3_0;
	// update uniform & draw
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, global3.m);
	glDrawArrays(GL_TRIANGLES, 0, teapot_vertex_count);

	//4th teapot
	//3nd tier
	mat4 local4_0 = identity_mat4();
	local4_0 = rotate_y_deg(local4_0, rotatez_tier_1 * 2)
		* translate(local4_0, vec3(0.0, getTierDisplacement(1), 0.0));
	mat4 global4 = root*local2_0*local3_0*local4_0;
	// update uniform & draw
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, global4.m);
	glDrawArrays(GL_TRIANGLES, 0, teapot_vertex_count);

	//5th teapot
	//4th tier
	mat4 local5_0 = identity_mat4();
	local5_0 = rotate_y_deg(local5_0, rotatez_tier_2 * 2) 
		* translate(local5_0, vec3(0.0, getTierDisplacement(1), 0.0));
	mat4 global5 = root*local2_0*local3_0*local4_0*local5_0;
	// update uniform & draw
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, global5.m);
	glDrawArrays(GL_TRIANGLES, 0, teapot_vertex_count);

	// 2nd teapot
	// child of hierarchy
	mat4 local2_1 = identity_mat4();
	local2_1 = rotate_y_deg(local2_1, rotatez_tier_1) 
		* translate(local2_1, vec3(-getTierDisplacement(1) / 4, -10, 0.0))
		* scale(local2_1, vec3(0.5f, 0.5f, 0.5f));
	mat4 global2_1 = root*local2_1;
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, global2_1.m);
	glDrawArrays(GL_TRIANGLES, 0, teapot_vertex_count);

	//3rd teapot
	//2nd tier
	mat4 local3_1 = identity_mat4();
	local3_1 = rotate_y_deg(local3_1, rotatez_tier_2) 
		* translate(local3_1, vec3(0.0, getTierDisplacement(1), 0.0));
	mat4 global3_1 = root*local2_1*local3_1;
	// update uniform & draw
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, global3_1.m);
	glDrawArrays(GL_TRIANGLES, 0, teapot_vertex_count);

	//4th teapot
	//3nd tier
	mat4 local4_1 = identity_mat4();
	local4_1 = rotate_y_deg(local4_1, rotatez_tier_1 * 2) 
		* translate(local4_1, vec3(0.0, getTierDisplacement(1), 0.0));
	mat4 global4_1 = root*local2_1*local3_1*local4_1;
	// update uniform & draw
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, global4_1.m);
	glDrawArrays(GL_TRIANGLES, 0, teapot_vertex_count);

	//5th teapot
	//4th tier
	mat4 local5_1 = identity_mat4();
	local5_1 = rotate_y_deg(local5_1, rotatez_tier_2 * 2) 
		* translate(local5_1, vec3(0.0, getTierDisplacement(1), 0.0));
	mat4 global5_1 = root*local2_1*local3_1*local4_1*local5_1;
	// update uniform & draw
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, global5_1.m);
	glDrawArrays(GL_TRIANGLES, 0, teapot_vertex_count);


	glutSwapBuffers();
}


void updateScene() {

	// Placeholder code, if you want to work with framerate
	// Wait until at least 16ms passed since start of last frame (Effectively caps framerate at ~60fps)
	static DWORD  last_time = 0;
	DWORD  curr_time = timeGetTime();
	float  delta = (curr_time - last_time) * 0.001f;
	if (delta > 0.03f)
		delta = 0.03f;
	last_time = curr_time;

	rotatez_tier_1 += 0.1f;
	rotatez_tier_2 -= 0.2f;
	// Draw the next frame
	glutPostRedisplay();
}


void init()
{
	// Set up the shaders
	GLuint shaderProgramID = CompileShaders();
	// load teapot mesh into a vertex buffer array
	generateObjectBufferTeapot();


	view = identity_mat4();
	persp_proj = perspective(60.0, (float)width / (float)height, 0.1, 100.0);
	base = identity_mat4();
	base = translate(base, vec3(0.0, 0.0, -60.0f)) * rotate_z_deg(base, 45.0f);

}

// Placeholder code for the keypress
void keypress(unsigned char key, int x, int y) {

	if (key == 27) {
		glutExit();
	} else if (key == 'w') {
		base = translate(base, vec3(0.0f, 1.0f, 0.0f));
	} else if (key == 's') {
		base = translate(base, vec3(0.0f, -1.0f, 0.0f));
	} else if (key == 'a') {
		base = translate(base, vec3(-1.0f, 0.0f, 0.0f));
	} else if (key == 'd') {
		base = translate(base, vec3(1.0f, 0.0f, 0.0f));
	} else if (key == 'q') {
		base = translate(base, vec3(0.0f, 0.0f, 1.0f));
	} else if (key == 'e') {
		base = translate(base, vec3(0.0f, 0.0f, -1.0f));
	}
}

int main(int argc, char** argv) {

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Hello Triangle");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keypress);

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











