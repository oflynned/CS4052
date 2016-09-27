#pragma once

#include <iostream>
#include <fstream>
#include <istream>

class FileReader
{
private:
	FileReader();
	~FileReader();

public:
	static std::string readFile(std::string filename);
};

