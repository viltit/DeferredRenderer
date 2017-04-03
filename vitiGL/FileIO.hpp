/* namespace for basic file handling */

#pragma once

#include <vector>
#include <string>

namespace vitiGL {

struct DirEntry {
	std::string path;
	bool isDir;
};

namespace FileIO {	
	bool dirEntries(const char* path, std::vector<DirEntry>& dirEntries);
}
}
