#pragma once
#include "glm.hpp"
#include "Cadfile.h"


using dvec3 = glm::dvec3;
using dvec4 = glm::dvec4;
using dvec2 = glm::dvec2;
using mat4 = glm::mat4;

inline dvec3 dfxlibToGlm(const dxflib::entities::vertex& v) {
    return dvec3(v.x, v.y, v.z);
}

class RotateCamera {
public:
    RotateCamera();
    void SetDistance(double d);
    void SetRotation(glm::quat q);
    void SetCenter(dvec3 center);
    mat4& CreatePerspectiveProjection();
    mat4 projMat, preRotTransMat, postRotTransMat, rotMat;
    double viewDistance;
    double fov, aspect, zNear, zFar;
};

class LinesDisplay {
public:
    LinesDisplay();
    ~LinesDisplay();
    void DrawLine(const dxflib::entities::line& line, dvec4 color = { 1.0,1.0,1.0,1.0 });
    void DrawLine(dvec3& start, dvec3& end, dvec4 color = { 1.0,1.0,1.0,1.0 });
    void DrawLine(const dxflib::entities::geoline& geoline, dvec4 color = { 1.0,1.0,1.0,1.0 });
    void DrawLine(const dxflib::entities::solid& solid, dvec4 color = { 1.0,1.0,1.0,1.0 });
    void Clear();
    void SetOffset(double x, double y);
    void SetScale(double s);
    void Display(RotateCamera& camera);

    int linesVaoElemCount = 0;
private:
    GLuint linesVAO, lineShader, linesVBO;
    std::vector<double> linesData;

    struct {
        GLuint rotation, preRotTranslation, postRotTranslation, projection, offsetScale;
    }uniform_loc;

    mat4 offsetScaleMat;
    double scale, offsetX, offsetY;
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

class AxisDisplay {
public:
    AxisDisplay();
    ~AxisDisplay();
    void Display(RotateCamera& camera);
    void SetPos(double x, double y);
    void SetSize(double sz);

    mat4 projMat, sizePosMat;
    LinesDisplay linesDisplay;
    RotateCamera dispCamera;
private:
    struct {
        GLuint rotMat, projMat, sizePosMat;
    }uniform_loc;

    GLuint VAO, VBO, lineShader;
};