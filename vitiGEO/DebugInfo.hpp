/*	singleton class to collect debug information from the physic engine 

	the w-component of the respective glm::vec4's is for storing the size */

#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace vitiGEO {

class DebugInfo {
public:
	static DebugInfo* instance();

	void clear() {
		_points.clear();
		_linesStart.clear();
		_linesEnd.clear();
	}

	void addPoint(const glm::vec4& point) { _points.push_back(point); }
	void addLine(const glm::vec4& lineStart, const glm::vec4& lineEnd) {
		_linesStart.push_back(lineStart);
		_linesEnd.push_back(lineEnd);
	}

	void addStaticLine(const glm::vec4& lineStart, const glm::vec4& lineEnd) {
		_sLinesStart.push_back(lineStart);
		_sLinesEnd.push_back(lineEnd);
	}

	std::vector<glm::vec4>& points()		{ return _points; }
	std::vector<glm::vec4>& linesStart()	{ return _linesStart; }
	std::vector<glm::vec4>& linesEnd()		{ return _linesEnd; }
	std::vector<glm::vec4> sLinesStart()	{ return _sLinesStart; }
	std::vector<glm::vec4> sLinesEnd()		{ return _sLinesEnd;  }

private:
	DebugInfo() {}

	std::vector<glm::vec4> _sLinesStart;
	std::vector<glm::vec4> _sLinesEnd;

	std::vector<glm::vec4> _points;
	std::vector<glm::vec4> _linesStart;
	std::vector<glm::vec4> _linesEnd;
};

}
