#include "ShapeLoader.hpp"
#include "Shape.hpp"
#include "Error.hpp"
#include "vitiGlobals.hpp"

#include <iostream>

namespace vitiGL {

namespace slSymbols {
	const char none = ' ';
	const char begin = '<';
	const char end = '>';
	const char terminator = '/';
	const std::string texture = "TEXTURE";
	const std::string dimensions = "DIMENSION";
	const std::string bools = "BOOLS";
	const std::string size = "SIZE";
	const std::string uv = "UV";
	const std::string diffuse = "DIFFUSE";
	const std::string specular = "SPECULAR";
	const std::string normal = "NORMAL";
	const std::string shape = "SHAPE";
}

/*	STRING BUFFER -------------------------------------------------------------- */
StringBuffer::StringBuffer(const std::string& file_path)
	: input{ std::ifstream{ file_path } },
	full{ false }
{
	if (!input) throw fileError(file_path.c_str());
#ifdef CONSOLE_LOG
	std::cout << "Opening file\t" << file_path << std::endl;
#endif
}

std::string	StringBuffer::get() {
	if (full) {
		full = false;
		std::string s{ value };
		value = "";
		return white(s);
	}
	std::getline(input, value);
	return white(value);
}

/* eliminate whitespace in front of a word */
std::string StringBuffer::white(const std::string line) {
	std::string newline;
	for (int i = 0; i < line.size(); i++) {
		if (!isspace(int(line[i]))) newline += line[i];
	}
	return newline;
}

float StringBuffer::getf() {
	std::string str = get();
	float f;
	try {
		f = std::stof(str);
	}
	catch (std::invalid_argument) {
		return 0.0f;
	}
	return f;
}

/* no safety against overwriting an existing value! */
void StringBuffer::putback(std::string s) {
	full = true;
	value = s;
}

void StringBuffer::putback(char c) {
	full = true;
	value += c;
}


/*	SHAPE LOADER -------------------------------------------------------------- */
ShapeLoader::ShapeLoader(slData & data, const std::string & filePath)
	:	buffer	{ filePath },
		path	{ filePath },
		texture	{ false }
{
	load(data);
	if (!texture) data.textures.clear();
}

ShapeLoader::~ShapeLoader() {
}

/* Main Switch */
void ShapeLoader::load(slData& data) {
	if (start_tag() == slSymbols::shape) {
		while (end_tag(slSymbols::shape)) {
			std::string in = start_tag();
			if (in == slSymbols::bools)		get_bools(data);
			else if (in == slSymbols::dimensions) get_dimensions(data);
			else if (in == slSymbols::texture)	get_texture(data);
			else throw initError(("<sParser::load>\tInvalid tag '" + in + "' in file " + path).c_str());
		}
	}
}

void ShapeLoader::get_bools(slData& data) {
	while (end_tag(slSymbols::bools)) {
		std::string line = buffer.get();

		/* we only have one boolean defined so far: */
		if (line == "Invert_Normals")
			data.invert = true;
		else
			throw initError(("<sParser::get_bools>\tInvalid identifier in config file " + path).c_str());
	}
}

void ShapeLoader::get_dimensions(slData& data) {
	/* because dimensions has 'sub-trees', we check for them first and switch: */
	while (end_tag(slSymbols::dimensions)) {
		std::string in = start_tag();
		if (in == slSymbols::size) {
			get_size(data);
		}
		else if (in == slSymbols::uv) {
			get_uv(data);
		}
		else throw initError(("<sParser::get_dimension>\tInvalid identifier in config file " + path).c_str());
	}
}

/* no safety net here: */
void ShapeLoader::get_size(slData& data) {
	while (end_tag(slSymbols::size)) {
		data.size.x = buffer.getf();
		data.size.y = buffer.getf();
		data.size.z = buffer.getf();
	}
}

void ShapeLoader::get_uv(slData& data) {
	while (end_tag(slSymbols::uv)) {
		data.uv.x = buffer.getf();
		data.uv.y = buffer.getf();
	}
}

/* lets us load more than one respective texture - not supported by shape yet*/
void ShapeLoader::get_texture(slData& data) {
	std::vector<std::string> diffuse;
	std::vector<std::string> specular;
	std::vector<std::string> normal;
	/* we immediatly search for sub-trees: */
	while (end_tag(slSymbols::texture)) {
		std::string in = start_tag();
		if (in == slSymbols::diffuse) {
			while (end_tag(slSymbols::diffuse)) diffuse.push_back(buffer.get());
		}
		else if (in == slSymbols::specular) {
			while (end_tag(slSymbols::specular)) specular.push_back(buffer.get());
		}
		else if (in == slSymbols::normal) {
			while (end_tag(slSymbols::normal)) normal.push_back(buffer.get());
		}
	}

	for (int i = 0; i < diffuse.size(); i++) {
		data.textures.push_back(diffuse[i]);
		if (specular.size() > i) data.textures.push_back(specular[i]);
		if (normal.size() > i) data.textures.push_back(normal[i]);
	}

	texture = true;
	return;
}


/* adds line for line into a vector of string until a string of the form </EXPRESSION> is found */
std::vector<std::string> ShapeLoader::get_string(const std::string& symbol) {
	std::vector<std::string> temp;
	while (end_tag(symbol)) {
		std::string line = buffer.get();
		temp.push_back(line);
	}
	return temp;
}

/*	Functions returns EXPRESSION out of <EXPRESSION> and throws an Exception if no '<' or '>'
	was found */
std::string ShapeLoader::start_tag() {
	/* check for correct grammar first, ie. '<' and '>' and no '</'		*/
	std::string line = buffer.get();
	if (line.length() < 2)
		throw initError(("<sParser::start_tag>\tStart tag " + line + " in file " + path + " is too small.").c_str());

	char ch1 = line[0];
	if (ch1 != slSymbols::begin)
		throw initError(("<sParser::start_tag>\tMissing '<' in file " + path + ", line: " + line).c_str());

	char ch2 = line[1];
	if (ch2 == slSymbols::terminator)
		throw initError(("<sParser::start_tag>\tUnexpected terminator in file " + path).c_str());

	char ch3 = line[line.size() - 1];
	if (ch3 != '>')
		throw initError(("<sParser::start_tag>\tMissing '>' in file " + path).c_str());

	/* grammar is correct, return EXPRESSION: */
	std::string middle;
	for (int i = 1; i < line.size() - 1; i++) {
		middle += line[i];
	}
	return middle;
}

/*	Function returns true if we find no </EXPRESSION> at all, false if we find one, and it throws
an Exception if we find an invalid grammar														*/

bool ShapeLoader::end_tag(const std::string& tag) {
	/* ensure proper grammar first, ie '</' and '>' */
	std::string line = buffer.get();
	if (line.length() < 2)
		throw initError(("<sParser::end_tag>\tEnd tag " + line + " in file " + path + " is too small.").c_str());

	char ch1 = line[0];
	if (ch1 != slSymbols::begin) {	/* check if we have a tag */
		buffer.putback(line);
		return true;
	}
	char ch2 = line[1];
	if (ch2 != slSymbols::terminator) { /* check for terminator */
		buffer.putback(line);
		return true;
	}
	char ch3 = line[line.size() - 1];	/* check for closing tag */
	if (ch3 != slSymbols::end) {
		buffer.putback(line);
		throw initError(("<sParser::end_tag>\tMissing '>' in file " + path).c_str());
	}

	std::string middle;
	for (int i = 2; i < line.size() - 1; i++)
		middle += line[i];

	if (middle != tag)			/* check if the tag corresponds to the opening tag */
		throw initError(("<sParser::end_tag>\tWrong closing Tag " + tag + " in File " + path).c_str());

	return false;
}

}