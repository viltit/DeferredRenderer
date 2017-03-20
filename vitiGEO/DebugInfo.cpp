#include "DebugInfo.hpp"

namespace vitiGEO {
DebugInfo* DebugInfo::instance() {
	static DebugInfo instance{};
	return &instance;
};

}
