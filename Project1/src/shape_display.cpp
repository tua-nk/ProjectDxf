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

void LinesDisplay::DrawLine(const dxflib::entities::geoline& geoline, dvec4 color)
{
    dvec3 start(geoline[0].x, geoline[0].y, geoline[0].z), end(geoline[1].x, geoline[1].y, geoline[1].z);
    DrawLine(start, end, color);
}

void LinesDisplay::DrawLine(const dxflib::entities::solid& solid, dvec4 color)
{
    dvec3 start1(solid.get_vertex(0).x, solid.get_vertex(0).y, solid.get_vertex(0).z), end1(solid.get_vertex(1).x, solid.get_vertex(1).y, solid.get_vertex(1).z);
    DrawLine(start1, end1, color);

    dvec3 start2(solid.get_vertex(1).x, solid.get_vertex(1).y, solid.get_vertex(1).z), end2(solid.get_vertex(2).x, solid.get_vertex(2).y, solid.get_vertex(2).z);
    DrawLine(start2, end2, color);

    dvec3 start3(solid.get_vertex(2).x, solid.get_vertex(2).y, solid.get_vertex(2).z), end3(solid.get_vertex(0).x, solid.get_vertex(0).y, solid.get_vertex(0).z);
    DrawLine(start3, end3, color);
}

void LinesDisplay::DrawLine(const dxflib::entities::line& line, dvec4 color) {
    const auto& vstart = line.get_vertex(0), vend = line.get_vertex(1);

    dvec3 start(vstart.x, vstart.y, vstart.z), end(vend.x, vend.y, vend.z);
    DrawLine(start, end, color);
}

void LinesDisplay::DrawLine(const std::vector<dxflib::entities::vertex> vertexs, dvec4 color)
{
    if (vertexs.size() < 2) return;
    for (int i = 0; i < vertexs.size()-1; i++)
    {
        dvec3 start(vertexs[i  ].x, vertexs[i  ].y, vertexs[i  ].z);
        dvec3 end  (vertexs[i+1].x, vertexs[i+1].y, vertexs[i+1].z);
        DrawLine(start, end, color);
    }
}

dxflib::entities::vertex evaluatePoint(const std::vector<dxflib::entities::vertex>& controlPoints, const std::vector<double>& knots, int degree, double t) {
    std::vector<dxflib::entities::vertex> d(controlPoints);  // Working copy of control points
    int n = controlPoints.size() - 1;
    int k = degree;
    
    // Find the knot span that contains t
    int i = k; // Start at first possible index
    for (; i <= n; ++i) {
        if (t < knots[i+1]) break;
    }
    if (i > n) i = n;  // Ensure i is within bounds if t is at the end of the knot vector

    // De Boor's Algorithm: Compute the point on the curve at parameter t
    for (int r = 1; r <= k; ++r) {
        for (int j = i; j > i - k + r - 1; --j) {
            if (j == 0) continue; // Prevent out-of-bounds access for d[j-1]
            double alpha = (t - knots[j]) / (knots[j + k - r + 1] - knots[j]);
            d[j] = d[j - 1] * (1 - alpha) + d[j] * alpha;
        }
    }
    return d[i];
}

// Function to compute a series of points on the B-spline curve
std::vector<dxflib::entities::vertex> computeBSplinePoints(const std::vector<dxflib::entities::vertex>& controlPoints, const std::vector<double>& knotVector, int degree, int numPoints) {
    std::vector<dxflib::entities::vertex> curvePoints;
    double t0 = knotVector[degree];
    double t1 = knotVector[knotVector.size() - degree - 1];
    double interval = (t1 - t0) / (numPoints - 1);

    for (int i = 0; i < numPoints; ++i) {
        double t = t0 + i * interval;
        dxflib::entities::vertex pt = evaluatePoint(controlPoints, knotVector, degree, t);
        curvePoints.push_back(pt);
    }
    return curvePoints;
}

void LinesDisplay::DrawLine(const dxflib::entities::spline spline, dvec4 color)
{
    std::vector<dxflib::entities::vertex> splinePoints = computeBSplinePoints(spline.get_control_points(), spline.get_knot_value(), spline.get_degree(), 100);
    DrawLine(splinePoints, color);
}

//std::vector<dxflib::entities::vertex> generateHelixPoints(float radius, float turnHeight, int numTurns, const dxflib::entities::vertex& start_point, const dxflib::entities::vertex& axisBase, const dxflib::entities::vertex& axisVector) {
//    std::vector<dxflib::entities::vertex> points;
//    float angleIncrement = 2.0f * dxflib::mathlib::pi;
//    int totalSteps = numTurns * 100;
//    float heightIncrement = turnHeight / 100;
//
//    // Normalize axis vector
//    glm::vec3 normAxis = glm::normalize(glm::vec3(axisVector.x, axisVector.y, axisVector.z));
//    glm::vec3 zAxis = glm::vec3(0, 0, 1);
//
//    // Calculate quaternion for rotation from z-axis to normAxis
//    glm::quat rotationQuat = glm::rotation(zAxis, normAxis);
//
//    // Convert quaternion to rotation matrix
//    glm::mat4 rotationMatrix = glm::toMat4(rotationQuat);
//
//    // Calculate translation to move helix base to axisBase
//    glm::vec3 translation = glm::vec3(axisBase.x - start_point.x, axisBase.y - start_point.y, axisBase.z - start_point.z);
//
//    for (int i = 0; i <= totalSteps; ++i) {
//        float currentAngle = i * angleIncrement / 100;
//        glm::vec4 pLocal = glm::vec4(radius * cos(currentAngle), radius * sin(currentAngle), i * heightIncrement, 1.0);
//        glm::vec4 pWorld = rotationMatrix * pLocal + glm::vec4(translation, 0.0);
//
//        points.push_back(dxflib::entities::vertex{ pWorld.x, pWorld.y, pWorld.z });
//    }
//    return points;
//}
std::vector<dxflib::entities::vertex> generateHelixPoints(float radius, float turnHeight, int numTurns, const dxflib::entities::vertex& start_point, const dxflib::entities::vertex& axisBase, const dxflib::entities::vertex& axisVector, bool isRightHanded) {
    std::vector<dxflib::entities::vertex> points;
    float angleIncrement = 2.0f * dxflib::mathlib::pi;
    int totalSteps = numTurns * 100;
    float heightIncrement = turnHeight / 100;

    // Normalize axis vector
    glm::vec3 normAxis = glm::normalize(glm::vec3(axisVector.x, axisVector.y, axisVector.z));
    glm::vec3 zAxis = glm::vec3(0, 0, 1);

    // Calculate quaternion for rotation from z-axis to normAxis
    glm::quat rotationQuat = glm::rotation(zAxis, normAxis);

    // Convert quaternion to rotation matrix
    glm::mat4 rotationMatrix = glm::toMat4(rotationQuat);

    // Calculate translation to move helix start to start_point
    glm::vec3 translation = glm::vec3(start_point.x - axisBase.x, start_point.y - axisBase.y, start_point.z - axisBase.z);

    // Adjust the angle increment based on the handedness
    if (!isRightHanded) {
        angleIncrement = -angleIncrement;  // Reverse the direction for left-handed helix
    }

    for (int i = 0; i <= totalSteps; ++i) {
        float currentAngle = i * angleIncrement / 100;
        glm::vec4 pLocal = glm::vec4(radius * cos(currentAngle), radius * sin(currentAngle), i * heightIncrement, 1.0);
        glm::vec4 pWorld = rotationMatrix * pLocal + glm::vec4(translation, 0.0) + glm::vec4(axisBase.x, axisBase.y, axisBase.z, 0.0);

        points.push_back(dxflib::entities::vertex{ pWorld.x, pWorld.y, pWorld.z });
    }
    return points;
}

void LinesDisplay::DrawLine(const dxflib::entities::helix helix, dvec4 color)
{
    std::vector<dxflib::entities::vertex> helixPoints = generateHelixPoints(helix.get_radius(), helix.get_turn_height(), helix.get_number_of_turns(), helix.get_start_point(), helix.get_axis_base_point(), helix.get_axis_vector(), helix.get_handedness());
    DrawLine(helixPoints, color);
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

PointDisplay::PointDisplay()
{
    glGenVertexArrays(1, &pointVAO);

    glGenBuffers(1, &pointVBO);

    pointShader = CreateShaderProgram("point");

    uniform_loc.projection = glGetUniformLocation(pointShader, "projMat");
    uniform_loc.preRotTranslation = glGetUniformLocation(pointShader, "preRotMat");
    uniform_loc.postRotTranslation = glGetUniformLocation(pointShader, "postRotMat");
    uniform_loc.rotation = glGetUniformLocation(pointShader, "rotMat");
    uniform_loc.offsetScale = glGetUniformLocation(pointShader, "offsetScaleMat");

    offsetScaleMat = glm::identity<mat4>();
    offsetX = 0;
    offsetY = 0;
    scale = 1.0;
}

PointDisplay::~PointDisplay()
{
    glDeleteVertexArrays(1, &pointVAO);
    glDeleteProgram(pointShader);
    glDeleteBuffers(1, &pointVBO);
}

void PointDisplay::DrawPoint(const dxflib::entities::point_base& point, dvec4 color)
{
    pointsData.push_back(point.x);
    pointsData.push_back(point.y);
    pointsData.push_back(point.z);

    pointsData.push_back(color.r);
    pointsData.push_back(color.g);
    pointsData.push_back(color.b);
    pointsData.push_back(color.a);

    pointVaoElemCount += 1;

    glBindVertexArray(pointVAO);

    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
    glBufferData(GL_ARRAY_BUFFER, pointsData.size() * sizeof(double), pointsData.data(), GL_STATIC_DRAW);
    glVertexAttribLPointer(0, 3, GL_DOUBLE, 7 * sizeof(double), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribLPointer(1, 4, GL_DOUBLE, 7 * sizeof(double), (void*)(3 * sizeof(double)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void PointDisplay::Clear()
{
    glBindVertexArray(pointVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);

    glBufferData(GL_ARRAY_BUFFER, pointsData.size() * sizeof(double), nullptr, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    pointsData.clear();
}

void PointDisplay::SetOffset(double x, double y)
{
    offsetX = x;
    offsetY = y;
    //offsetScaleMat = glm::translate(glm::vec3(-offsetX,-offsetY,0)) * glm::scale(glm::vec3(scale));
    offsetScaleMat = glm::translate(glm::vec3(-offsetX, -offsetY, 0));
    offsetScaleMat[0][0] *= scale;
    offsetScaleMat[1][1] *= scale;
    offsetScaleMat[2][2] *= scale;
}

void PointDisplay::SetScale(double s)
{
    scale = s;
    //offsetScaleMat = glm::translate(glm::vec3(-offsetX,-offsetY,0)) * glm::scale(glm::vec3(scale));
    offsetScaleMat = glm::translate(glm::vec3(-offsetX, -offsetY, 0));
    offsetScaleMat[0][0] *= scale;
    offsetScaleMat[1][1] *= scale;
    offsetScaleMat[2][2] *= scale;
}

void PointDisplay::Display(RotateCamera& camera)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glDepthFunc(GL_LESS);

    glBindVertexArray(pointVAO);
    glUseProgram(pointShader);

    glUniformMatrix4fv(uniform_loc.projection, 1, GL_FALSE, &camera.projMat[0][0]);
    glUniformMatrix4fv(uniform_loc.rotation, 1, GL_FALSE, &camera.rotMat[0][0]);
    glUniformMatrix4fv(uniform_loc.postRotTranslation, 1, GL_FALSE, &camera.postRotTransMat[0][0]);
    glUniformMatrix4fv(uniform_loc.preRotTranslation, 1, GL_FALSE, &camera.preRotTransMat[0][0]);
    glUniformMatrix4fv(uniform_loc.offsetScale, 1, GL_FALSE, &offsetScaleMat[0][0]);

    glLineWidth(1);
    glDrawArrays(GL_POINTS, 0, pointVaoElemCount);
}

RotateCamera::RotateCamera()
{
    projMat = mat4(0.0);
    preRotTransMat = mat4(0.0);
    postRotTransMat = mat4(0.0);
    rotMat = mat4(0.0);
    viewDistance = 0.0;
    fov = 0.0;
    aspect = 0.0;
    zNear = 0.0;
    zFar = 0.0;
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