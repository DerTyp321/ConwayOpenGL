#ifndef FILEUTIL
#define FILEUTIL

#include <string>
#include <fstream>
#include <iostream>

std::string loadFile(std::string path) {
	std::ifstream stream(path);
	if (!stream) {
		std::cerr << "couldn't load file: " << path << std::endl;
	}
	std::string content;
	stream.seekg(0, std::ios::end);
	content.reserve(stream.tellg());
	stream.seekg(0, std::ios::beg);
	content.assign((std::istreambuf_iterator<char>(stream)),
			std::istreambuf_iterator<char>());
	//std::cout << "File: \n" << content << std::endl;
	return content;
}

#endif
