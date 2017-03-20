/*	
	this class reads, compiles and links OpenGL Shader Code 

	after succesfull linking, it is a handler to the OpenGL Programm which can be turned
	on and off with the respective functions

	EXCEPTIONS:
		-> Constructor can throw a shaderError and a fileError
		-> getUniform can throw a shaderError
*/

#pragma once

#include <string>
#include <GL/glew.h>

namespace vitiGL {

class Shader {
public:
	Shader() {};

	/* use this constructor if in doubt */
	Shader(const std::string& vertexPath, const std::string& fragmentPath,
		   const std::string& geoPath = "", const std::string& tessPath = "");
	~Shader();

	void	on() const;
	void	off() const;

	GLint	getUniform(const std::string& name) const;

	GLuint	program() const { return programID; }


	void	init(const std::string& vertexPath, 
				 const std::string& fragmentPath,
				 const std::string& geoPath = "", 
				 const std::string& tessPath = "");

	void	compile(const std::string& filePath, GLint shaderID);

	void	link();

private:
	GLint	programID;

	GLint	vertexID;
	GLint	fragmentID;
	GLint	geoID;
	GLint	tessID;
};
}
