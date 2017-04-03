#include "FileIO.hpp"
#include <experimental/filesystem>

//shortcut for filesystem namespace:
namespace fs = std::experimental::filesystem;

namespace vitiGL {
namespace FileIO {
	
	bool dirEntries(const char* path, std::vector<DirEntry>& dirEntries) {
		auto dpath = fs::path(path);
		
		if (!fs::is_directory(dpath)) return false;
		
		for (auto i = fs::directory_iterator(dpath); i != fs::directory_iterator(); ++i) {
			dirEntries.emplace_back();
			dirEntries.back().path = i->path().string();
			if (fs::is_directory(i->path())) dirEntries.back().isDir = true;
			else dirEntries.back().isDir = false;
		}

		return true;
	}	
}
}
