#include "vitiTypes.hpp"

namespace vitiGL {

//overload << operator for vec3's - for console output:
std::ostream& operator << (std::ostream& os, const glm::vec3& v) {
	os << v.x << " / " << v.y << " / " << v.z;
	return os;
}
}