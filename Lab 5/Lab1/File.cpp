#pragma once

#include <string>

class File {
public:
	static const std::string getAbsoluteModelPath(std::string path) {
		return "U:/CS4052/Lab1/Lab1/Models/" + path;
	}

	static const std::string getAbsoluteSoundPath(std::string path) {
		return "U:/CS4052/Lab1/Lab1/Sounds/" + path;
	}

	static const std::string getAbsoluteShaderPath(const std::string path) {
		return "U:/CS4052/Lab1/Lab1/Shaders/" + path;
	}

	static const std::string getRelativePath(std::string path) {
		return "../Lab1/" + path;
	}
};