#include "Shader.hpp"
#include "Error.hpp"

#include <fstream>
#include <vector>
#include <iostream>

namespace vitiGL {

Shader::Shader(const std::string & vertexPath, const std::string & fragmentPath,
			   const std::string & geoPath, const std::string & tessPath)
	:	programID	{ 0 },
		vertexID	{ 0 },
		fragmentID	{ 0 },
		geoID		{ 0 },
		tessID		{ 0 }
{
	init(vertexPath, fragmentPath, geoPath, tessPath);
}

Shader::~Shader() {
	glDeleteProgram(programID);
	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);
	if (geoID != 0) glDeleteShader(geoID);
	if (tessID != 0) glDeleteShader(tessID);
}

void Shader::on() const {
	glUseProgram(programID);
}

void Shader::off() const {
	glUseProgram(0);
}

GLint Shader::getUniform(const std::string & name) const {
	GLint location = glGetUniformLocation(programID, name.c_str());
	if (location == GL_INVALID_INDEX) {
		std::string message = "Uniform " + name + " not found";
		std::cout << message << std::endl;
		//throw shaderError(message.c_str());
	}
	return location;
}


/* TO DO: CHECK IF GL NEEDS TWO TESSELATION SHADERS */
void Shader::init(const std::string& vertexPath, const std::string& fragmentPath,
					 const std::string& geoPath, const std::string& tessPath)
{
	//Compile vertex and fragment shader:
	programID = glCreateProgram();
	vertexID = glCreateShader(GL_VERTEX_SHADER);
	fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

	if (programID == 0 || vertexID == 0 || fragmentID == 0) throw shaderError("OpenGL could not create Shader identifiers.");

	compile(vertexPath, vertexID);
	compile(fragmentPath, fragmentID);
	
	//if geometry or tesselation shader is given, compile them too:
	if (geoPath != "") {
		geoID = glCreateShader(GL_GEOMETRY_SHADER);
		if (geoID == 0) throw shaderError("OpenGL could not create an identifier for the geometry shader.");
		compile(geoPath, geoID);
	}
	if (tessPath != "") {
		tessID = glCreateShader(GL_TESS_CONTROL_SHADER);
		if (tessID == 0) throw shaderError("OpenGL could not create an identifier for the tesselation control shader");
		compile(tessPath, tessID);
	}

	//Now that all shaders are compiled, link the program:
	link();
}

void Shader::compile(const std::string& filePath, GLint shaderID) {

	//Open a stream to the shader file:
	std::fstream shaderFile{ filePath };
	if (!shaderFile) throw fileError(filePath.c_str());

	//Read the file line by line:
	std::string content;
	std::string line;

	while (std::getline(shaderFile, line)) {
		content += line + "\n";
	}

	//Compile the shader:
	const char* contentPointer = content.c_str();
	glShaderSource(shaderID, 1, &contentPointer, nullptr);
	glCompileShader(shaderID);

	//Test for success:
	GLint success{ 0 };
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	
	if (!success) {
		GLint length;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> log(length);
		glGetShaderInfoLog(shaderID, length, &length, &(log[0]));
		glDeleteShader(shaderID);
		throw shaderError(&(log[0]));
	}
}

void Shader::link() {

	//Attach Shaders to the program:
	glAttachShader(programID, vertexID);
	glAttachShader(programID, fragmentID);
	if (geoID != 0) glAttachShader(programID, geoID); //it WOULD be possible to attach an empty shader...
	if (tessID != 0) glAttachShader(programID, tessID);

	//link the Program:
	glLinkProgram(programID);

	//Check for success:
	GLint success{ 0 };
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success) {
		GLint length;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> log(length);
		glGetProgramInfoLog(programID, length, &length, &(log[0]));

		throw shaderError(&(log[0]));
	}
}

}