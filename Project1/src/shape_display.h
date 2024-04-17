#pragma once
#include "glm.hpp"
#include "Cadfile.h"


using dvec3 = glm::dvec3;
using dvec2 = glm::dvec2;
using mat4 = glm::mat4;

inline dvec3 dfxlibToGlm(const dxflib::entities::vertex& v) {
	return dvec3(v.x, v.y, v.z);
}

class RotateCamera {
public:
	void SetDistance(double d);
	void SetRotation(glm::quat q);
	void SetCenter(dvec3 center);

	mat4 projMat, preRotTransMat, postRotTransMat, rotMat;
	double viewDistance;
};

class LinesDisplay {
public:
	LinesDisplay();
	~LinesDisplay();
	void DrawLine(dxflib::entities::line& line);
	void DrawLine(dvec3& start, dvec3& end);
	void Clear();
	void Display(RotateCamera& camera);

	int linesVaoElemCount = 0;
private:
	GLuint linesVAO, lineShader, linesVBO;
	std::vector<double> linesData;

	struct {
		GLuint rotation, preRotTranslation, postRotTranslation, projection;
	}uniform_loc;
};

class ArcsDisplay {
public:
	ArcsDisplay();
	~ArcsDisplay();
	void Display(RotateCamera& camera);
	void Clear();
	void DrawArc(dvec3& center, double radius, double radStart, double radEnd, bool ccw = true);
	void DrawArc(const dxflib::entities::arc& arc);
	void DrawArc(const dxflib::entities::circle& circle);

	LinesDisplay linesDisplay;
};