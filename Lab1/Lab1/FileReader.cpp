#include "FileReader.h"

FileReader::FileReader()
{
}


FileReader::~FileReader()
{
}

std::string FileReader::readFile(std::string filename) {
	std::ifstream ifstream;
	ifstream.open(filename);

	std::stringstream strStream;
	strStream << ifstream.rdbuf();
	return strStream.str();
