#pragma once

#include "GLIncludes.h"
#include <fstream>
#include <sstream>

class Shader {
private:
	GLuint program;

public:
	Shader() {}
	Shader(const GLchar* vertexPath, const GLchar* fragPath) {
		std::string vertexCode, fragmentCode;
		std::ifstream vShaderFile, fShaderFile;

		vShaderFile.exceptions(std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::badbit);

		try {
			std::stringstream vShaderStream;
			vShaderFile.open(vertexPath);
			vShaderStream << vShaderFile.rdbuf();
			vShaderFile.close();
			vertexCode = vShaderStream.str();

			std::stringstream fShaderStream;
			fShaderFile.open(fragPath);
			fShaderStream << fShaderFile.rdbuf();
			fShaderFile.close();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e) {
			std::cout << "Error in reading in shader file" << std::endl;
		}

		const GLchar* vShaderCode = vertexCode.c_str();
		const GLchar* fShaderCode = fragmentCode.c_str();

		GLuint vertex, fragment;
		GLint success;
		GLchar log[512];

		// compile vertex shader from code
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

		if (!success) {
			glGetShaderInfoLog(vertex, 512, NULL, log);
			std::cout << "Compilation failure: " << log << std::endl;
		}

		// compile fragment shader from code
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

		if (!success) {
			glGetShaderInfoLog(fragment, 512, NULL, log);
			std::cout << "Compilation failure: " << log << std::endl;
		}

		this->program = glCreateProgram();
		glAttachShader(this->program, vertex);
		glAttachShader(this->program, fragment);
		glLinkProgram(this->program);

		// throw linker errors
		glGetProgramiv(this->program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(this->program, 512, NULL, log);
			std::cout << "Linker failure: " << log << std::endl;
		}

		// shaders now linked to program and now can be deleted
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	~Shader() {}

	void use() {
		glUseProgram(this->program);
	}

	GLint getProgram() {
		return this->program;
	}
};