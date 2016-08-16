#pragma once

namespace vitiGL {

struct shaderError {
	shaderError(const char* message);
	~shaderError() {};
};

struct fileError {
	fileError(const char* path);
	~fileError() {};
};

struct initError {
	initError(const char* messageA, const char* messageB);
	initError(const char* message);
	~initError() {};
};

}

