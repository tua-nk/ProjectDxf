#include "gl_obj.h"
#include "shape_display.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm.hpp"
#include "gtx\transform.hpp"
#include "gtc\quaternion.hpp"
#include "gtx\quaternion.hpp"
#include <fstream>
#include <sstream>


/***************************************
*LINES DISPLAYER
****************************************/

LinesDisplay::LinesDisplay() {
	glGenVertexArrays(1, &linesVAO);

	glGenBuffers(1, &linesVBO);

	lineShader =  CreateShaderProgram("line");

	uniform_loc.projection = glGetUniformLocation(lineShader, "projMat");
	uniform_loc.preRotTranslation = glGetUniformLocation(lineShader, "preRotTransMat");
	uniform_loc.postRotTranslation = glGetUniformLocation(lineShader, "postRotTransMat");
	uniform_loc.rotation = glGetUniformLocation(lineShader, "rotMat");

}

LinesDisplay::~LinesDisplay() {
	glDeleteVertexArrays(1, &linesVAO);
	glDeleteProgram(lineShader);
	glDeleteBuffers(1, &linesVBO);
}

void LinesDisplay::Display(RotateCamera& camera) {
	glBindVertexArray(linesVAO);
	glUseProgram(lineShader);

	glUniformMatrix4fv(uniform_loc.projection, 1, GL_FALSE, &camera.projMat[0][0]);
	glUniformMatrix4fv(uniform_loc.rotation, 1, GL_FALSE, &camera.rotMat[0][0]);
	glUniformMatrix4fv(uniform_loc.postRotTranslation, 1, GL_FALSE, &camera.postRotTransMat[0][0]);
	glUniformMatrix4fv(uniform_loc.preRotTranslation, 1, GL_FALSE, &camera.preRotTransMat[0][0]);

	glDrawArrays(GL_LINES, 0, linesVaoElemCount);
}

void LinesDisplay::DrawLine(dvec3& start, dvec3& end) {
	linesData.push_back(start.x);
	linesData.push_back(start.y);
	linesData.push_back(start.z);

	linesData.push_back(1.0);
	linesData.push_back(1.0);
	linesData.push_back(1.0);
	linesData.push_back(1.0);

	linesData.push_back(end.x);
	linesData.push_back(end.y);
	linesData.push_back(end.z);

	linesData.push_back(1.0);
	linesData.push_back(1.0);
	linesData.push_back(1.0);
	linesData.push_back(1.0);

	linesVaoElemCount += 2;

	glBindVertexArray(linesVAO);
	glBindVertexArray(linesVAO);

	glBindBuffer(GL_ARRAY_BUFFER, linesVBO);
	glBufferData(GL_ARRAY_BUFFER, linesData.size() * sizeof(double), linesData.data(), GL_STATIC_DRAW);
	glVertexAttribLPointer(0, 3, GL_DOUBLE, 7 * sizeof(double), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribLPointer(1, 4, GL_DOUBLE, 7 * sizeof(double), (void*)(3 * sizeof(double)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void LinesDisplay::DrawLine(dxflib::entities::line& line) {
	const auto& vstart = line.get_vertex(0), vend = line.get_vertex(1);

	dvec3 start(vstart.x, vstart.y, vstart.z), end(vend.x, vend.y, vend.z);
	DrawLine(start, end);
}

void LinesDisplay::Clear() {
	glBindVertexArray(linesVAO);
	glBindBuffer(GL_ARRAY_BUFFER, linesVBO);

	glBufferData(GL_ARRAY_BUFFER, linesData.size() * sizeof(double), nullptr, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	linesData.clear();
}


void RotateCamera::SetDistance(double d) {
	viewDistance = glm::abs(d);
	postRotTransMat = glm::translate(glm::vec3(0.0f, 0.0f, -viewDistance));
}

void RotateCamera::SetCenter(dvec3 center) {
	preRotTransMat = glm::translate(-center);
}

void RotateCamera::SetRotation(glm::quat q) {
	rotMat = glm::toMat4(q);
}

/***************************************
* ARCS DISPLAYER
****************************************/

ArcsDisplay::ArcsDisplay()
{

}
ArcsDisplay::~ArcsDisplay() {

}

void ArcsDisplay::DrawArc(dvec3& center, double radius, double radStart, double radEnd, bool ccw)
{
	constexpr double angle_q = 0.015625 * glm::pi<double>();
	const dvec3 v1(radius, 0.0, 0.0), v2(0.0, -radius, 0.0);

	dvec3 p1 = ccw ?
		center + v1 * glm::cos(radStart) + v2 * glm::sin(radStart) :
		center + v1 * glm::cos(radStart) - v2 * glm::sin(radStart);

	for (double a = radStart + angle_q; a < radEnd; a += angle_q)
	{
		dvec3 p2 = ccw ?
			 center + v1 * glm::cos(a) + v2 * glm::sin(a) :
			 center + v1 * glm::cos(a) - v2 * glm::sin(a);

		linesDisplay.DrawLine(p1, p2);
		p1 = p2;
	}

	dvec3 p2 = ccw ?
		center + v1 * glm::cos(radEnd) + v2 * glm::sin(radEnd) :
		center + v1 * glm::cos(radEnd) - v2 * glm::sin(radEnd);

	linesDisplay.DrawLine(p1, p2);
}

void ArcsDisplay::DrawArc(const dxflib::entities::arc& arc)
{
	dvec3 center = dfxlibToGlm(arc.get_center_point());

	DrawArc(center, arc.get_radius(), glm::radians(arc.get_start_angle()), glm::radians(arc.get_end_angle()), arc.is_counterclock_wise());
}


void ArcsDisplay::DrawArc(const dxflib::entities::circle& circle)
{
	DrawArc(dfxlibToGlm(circle.get_center_point()), circle.get_radius(), 0, glm::pi<double>() * 2);
}

void ArcsDisplay::Display(RotateCamera& camera)
{
	linesDisplay.Display(camera);
}
void ArcsDisplay::Clear()
{
	linesDisplay.Clear();
}
