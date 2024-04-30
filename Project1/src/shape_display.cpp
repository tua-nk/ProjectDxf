#include "gl_obj.h"
#include "shape_display.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm.hpp"
#include "gtx\transform.hpp"
#include "gtc\quaternion.hpp"
#include "gtx\quaternion.hpp"
#include <fstream>
#include <sstream>
#include "imgui.h"

LinesDisplay::LinesDisplay() {
    glGenVertexArrays(1, &linesVAO);

    glGenBuffers(1, &linesVBO);

    lineShader = CreateShaderProgram("line");

    uniform_loc.projection = glGetUniformLocation(lineShader, "projMat");
    uniform_loc.preRotTranslation = glGetUniformLocation(lineShader, "preRotMat");
    uniform_loc.postRotTranslation = glGetUniformLocation(lineShader, "postRotMat");
    uniform_loc.rotation = glGetUniformLocation(lineShader, "rotMat");
    uniform_loc.offsetScale = glGetUniformLocation(lineShader, "offsetScaleMat");

    offsetScaleMat = glm::identity<mat4>();
    offsetX = 0;
    offsetY = 0;
    scale = 1.0;

}

LinesDisplay::~LinesDisplay() {
    glDeleteVertexArrays(1, &linesVAO);
    glDeleteProgram(lineShader);
    glDeleteBuffers(1, &linesVBO);
}

void LinesDisplay::Display(RotateCamera& camera) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glBindVertexArray(linesVAO);
    glUseProgram(lineShader);

    glUniformMatrix4fv(uniform_loc.projection, 1, GL_FALSE, &camera.projMat[0][0]);
    glUniformMatrix4fv(uniform_loc.rotation, 1, GL_FALSE, &camera.rotMat[0][0]);
    glUniformMatrix4fv(uniform_loc.postRotTranslation, 1, GL_FALSE, &camera.postRotTransMat[0][0]);
    glUniformMatrix4fv(uniform_loc.preRotTranslation, 1, GL_FALSE, &camera.preRotTransMat[0][0]);
    glUniformMatrix4fv(uniform_loc.offsetScale, 1, GL_FALSE, &offsetScaleMat[0][0]);

    glLineWidth(1);
    glDrawArrays(GL_LINES, 0, linesVaoElemCount);
}

void LinesDisplay::DrawLine(dvec3& start, dvec3& end, dvec4 color) {
    linesData.push_back(start.x);
    linesData.push_back(start.y);
    linesData.push_back(start.z);

    linesData.push_back(color.r);
    linesData.push_back(color.g);
    linesData.push_back(color.b);
    linesData.push_back(color.a);

    linesData.push_back(end.x);
    linesData.push_back(end.y);
    linesData.push_back(end.z);

    linesData.push_back(color.r);
    linesData.push_back(color.g);
    linesData.push_back(color.b);
    linesData.push_back(color.a);

    linesVaoElemCount += 2;

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

void LinesDisplay::DrawLine(dxflib::entities::line& line, dvec4 color) {
    const auto& vstart = line.get_vertex(0), vend = line.get_vertex(1);

    dvec3 start(vstart.x, vstart.y, vstart.z), end(vend.x, vend.y, vend.z);
    DrawLine(start, end, color);
}

void LinesDisplay::Clear() {
    glBindVertexArray(linesVAO);
    glBindBuffer(GL_ARRAY_BUFFER, linesVBO);

    glBufferData(GL_ARRAY_BUFFER, linesData.size() * sizeof(double), nullptr, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    linesData.clear();
}

void LinesDisplay::SetOffset(double x, double y)
{
    offsetX = x;
    offsetY = y;
    //offsetScaleMat = glm::translate(glm::vec3(-offsetX,-offsetY,0)) * glm::scale(glm::vec3(scale));
    offsetScaleMat = glm::translate(glm::vec3(-offsetX, -offsetY, 0));
    offsetScaleMat[0][0] *= scale;
    offsetScaleMat[1][1] *= scale;
    offsetScaleMat[2][2] *= scale;
}

void LinesDisplay::SetScale(double s)
{
    scale = s;
    //offsetScaleMat = glm::translate(glm::vec3(-offsetX,-offsetY,0)) * glm::scale(glm::vec3(scale));
    offsetScaleMat = glm::translate(glm::vec3(-offsetX, -offsetY, 0));
    offsetScaleMat[0][0] *= scale;
    offsetScaleMat[1][1] *= scale;
    offsetScaleMat[2][2] *= scale;
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

mat4& RotateCamera::CreatePerspectiveProjection() {
    projMat = glm::perspective<float>(fov, aspect, zNear, zFar);
    return projMat;
}


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

    if (radStart > radEnd)
    {
        radStart -= 2 * dxflib::mathlib::pi;
    }

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




AxisDisplay::AxisDisplay()
{
    lineShader = CreateShaderProgram("axis");

    uniform_loc.projMat = glGetUniformLocation(lineShader, "projMat");
    uniform_loc.sizePosMat = glGetUniformLocation(lineShader, "sizePosMat");
    uniform_loc.rotMat = glGetUniformLocation(lineShader, "rotMat");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    double vertData[] = {
        // line
        0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
        1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0,
        0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0,


        1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
        0.85, 0.0, -0.09, 1.0, 0.0, 0.0, 1.0,
        1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
        0.85, 0.0, 0.09, 1.0, 0.0, 0.0, 1.0,

        0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0,
        0.09, 0.85, 0.0, 0.0, 1.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0,
        -0.09, 0.85, 0.0, 0.0, 1.0, 0.0, 1.0,

        0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0,
        0.09, 0.0, 0.85, 0.0, 0.0, 1.0, 1.0,
        0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0,
        -0.09, 0.0, 0.85, 0.0, 0.0, 1.0, 1.0
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertData), vertData, GL_STATIC_DRAW);

    glVertexAttribLPointer(0, 3, GL_DOUBLE, 7 * sizeof(double), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribLPointer(1, 4, GL_DOUBLE, 7 * sizeof(double), (void*)(3 * sizeof(double)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    projMat = glm::perspective(45.0, 4.0 / 3.0, 0.0001, 10000.0);
    sizePosMat = glm::identity<mat4>();
}

AxisDisplay::~AxisDisplay()
{
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(lineShader);
}

void AxisDisplay::Display(RotateCamera& camera)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glBindVertexArray(VAO);
    glUseProgram(lineShader);

    glUniformMatrix4fv(uniform_loc.projMat, 1, GL_FALSE, &camera.projMat[0][0]);
    glUniformMatrix4fv(uniform_loc.rotMat, 1, GL_FALSE, &camera.rotMat[0][0]);
    glUniformMatrix4fv(uniform_loc.sizePosMat, 1, GL_FALSE, &sizePosMat[0][0]);

    glLineWidth(3);
    glDrawArrays(GL_LINES, 0, 6);

    glUniformMatrix4fv(uniform_loc.projMat, 1, GL_FALSE, &camera.projMat[0][0]);
    glUniformMatrix4fv(uniform_loc.rotMat, 1, GL_FALSE, &camera.rotMat[0][0]);
    glUniformMatrix4fv(uniform_loc.sizePosMat, 1, GL_FALSE, &sizePosMat[0][0]);
    glDrawArrays(GL_LINES, 6, 12);
}

void AxisDisplay::SetPos(double x, double y)
{
    sizePosMat[3].x = x;
    sizePosMat[3].y = y;
}

void AxisDisplay::SetSize(double sz)
{
    sizePosMat[0][0] = sz;
    sizePosMat[1][1] = sz;
    sizePosMat[2][2] = sz;
}