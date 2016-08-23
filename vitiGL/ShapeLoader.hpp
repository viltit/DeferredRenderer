/*	
	Simple parser for simple Shapes 
	reads texture paths, dimensions and uv-dimensions from an xml file
	-	only the ShapeLoaders constructor needs to be calles. It will
		fill all data into the slData Struct delivered to it

	may grow into a more advanced state later
*/

#pragma once

#include <fstream>
#include <vector>


namespace vitiGL {

/* forward declaration for data exchange struct: */
struct slData;

/* define legal tags for config file */
namespace slSymbols {
	extern const char none;
	extern const char begin;
	extern const char end;
	extern const char terminator;
	extern const std::string texture;
	extern const std::string dimensions;
	extern const std::string bools;
	extern const std::string size;
	extern const std::string uv;
	extern const std::string diffuse;
	extern const std::string specular;
	extern const std::string normal;
	extern const std::string shape;
}

/*	struct for buffering the input stream into a string
(the string is easyier to handle)					*/
struct StringBuffer {
	StringBuffer(const std::string& file_path);
	std::string		get();
	float			getf();

	/* no safety against overwriding: */
	void			putback(std::string s);
	void			putback(char x);
	std::string		white(const std::string line);

	std::string		value;
	bool			full;
	std::ifstream	input;
};

class ShapeLoader {

public:
	ShapeLoader(slData& data, const std::string& filePath);
	~ShapeLoader();

private:
	void				load(slData& data);
	void				get_texture(slData& data);
	void				get_dimensions(slData& data);
	void				get_bools(slData& data);
	void				get_size(slData& data);
	void				get_uv(slData& data);

	std::vector<std::string> get_string(const std::string& symbol);

	StringBuffer		buffer;

	std::string			path;

	std::string			start_tag();
	bool				end_tag(const std::string& tag);

	bool				texture;
};
}
