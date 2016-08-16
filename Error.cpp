#include "Error.hpp"

#include <iostream>

namespace vitiGL {

shaderError::shaderError(const char* message) {
	std::cout << message << std::endl;
}

fileError::fileError(const char * path) {
	std::cout << "Failed to open file " << path << "." << std::endl;
}

initError::initError(const char* messageA, const char* messageB) {
	std::cout << messageA << std::endl << messageB << std::endl;
}

initError::initError(const char * message) {
	std::cout << message << std::endl;
}
}