#include <glad\glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends\imgui_impl_glfw.h"
#include "backends\imgui_impl_opengl3.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <functional>
#include <unordered_map>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm.hpp"
#include "gtx\transform.hpp"
#include "gtc\quaternion.hpp"
#include "gtx\quaternion.hpp"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <commdlg.h>
#include "shape_display.h"

#include "Cadfile.h"

void gassert_()
{
    if (glGetError() != GL_NO_ERROR)
    {
        std::string error;
        auto er = glGetError();
        switch (er)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }

        std::cout << error;
    }
}

using dvec3 = glm::dvec3;
using dvec2 = glm::dvec2;
using mat4 = glm::mat4;

enum class KeyState {
    Up, Down, JustPressed, JustReleased
};

std::unordered_map<int, KeyState> keyStates;
double mouseScrollX = 0, mouseScrollY = 0;
bool cursorMoved = false;
dvec2 currentCursor(0), lastCursor(0);

struct WindowInfo {
    bool glInit = false;
    bool resized = false;
    void NewFrameState() {
        resized = false;
    }
};

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    cursorMoved = true;
    currentCursor = dvec2(xpos, ypos);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        keyStates[button] = KeyState::JustPressed;
    }
    else if (action == GLFW_RELEASE)
    {
        keyStates[button] = KeyState::JustReleased;
    }
}

void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    mouseScrollY = yoffset;
    mouseScrollX = xoffset;
}

void WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    WindowInfo* winInfo = static_cast<WindowInfo*>(glfwGetWindowUserPointer(window));
    if (winInfo)
    {
        if (winInfo->glInit)
        {
            int frameW, frameH;
            glfwGetFramebufferSize(window, &frameW, &frameH);
            glViewport(0, 0, frameW, frameH);
        }
        winInfo->resized = true;
    }
}

void NewFrameInputStates() {
    for (auto it = keyStates.begin(); it != keyStates.end(); ++it)
    {
        if (it->second == KeyState::JustPressed)
        {
            it->second = KeyState::Down;
        }
        else if (it->second == KeyState::JustReleased)
        {
            it->second = KeyState::Up;
        }
    }
    cursorMoved = false;
    lastCursor = currentCursor;
    mouseScrollX = mouseScrollY = 0;
}

bool IsDown(int button)
{
    return keyStates[button] == KeyState::Down || keyStates[button] == KeyState::JustPressed;
}

void Play(GLFWwindow* window);

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    WindowInfo windowInfo;
    glfwSetWindowUserPointer(window, &windowInfo);

    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetScrollCallback(window, MouseScrollCallback);
    glfwSetWindowSizeCallback(window, WindowSizeCallback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    windowInfo.glInit = true;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // play program
    Play(window);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}

void Play(GLFWwindow* window)
{
    WindowInfo* winInfo = static_cast<WindowInfo*>(glfwGetWindowUserPointer(window));
    LinesDisplay linesDisplay;
    ArcsDisplay arcsDisplay;
    PointDisplay pointDisplay;

    RotateCamera rotateCamera;

    glm::dquat rotation(dvec3(0.0, 0.0, 0.0));
    rotateCamera.rotMat = glm::toMat4(rotation);

    rotateCamera.postRotTransMat = glm::translate(dvec3(0, 0, -5.0));
    rotateCamera.preRotTransMat = glm::translate(dvec3(0));

    double viewDistance = 5.0;
    rotateCamera.SetDistance(viewDistance);

    rotateCamera.fov = 45.0;
    {
        int w, h;
        glfwGetWindowSize(window, &w, &h);
        rotateCamera.aspect = double(w) / h;
        rotateCamera.zNear = 0.0001;
        rotateCamera.zFar = 10000.0;
        rotateCamera.CreatePerspectiveProjection();
    }

    HWND win32window = glfwGetWin32Window(window);
    OPENFILENAMEA ofn;
    CHAR szFile[MAX_PATH] = { 0 };
    CHAR fileTitle[256] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "(*.dxf)\0*.dxf\0";
    ofn.lpstrFileTitle = fileTitle;
    ofn.nMaxFileTitle = 256;
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = "Select a File";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING;

    AxisDisplay axisDisplay;
    axisDisplay.SetPos(-0.75, -0.75);
    axisDisplay.SetSize(0.4);

    std::string imageInfo;

    while (!glfwWindowShouldClose(window))
    {
        if (cursorMoved && keyStates[GLFW_MOUSE_BUTTON_LEFT] == KeyState::Down)
        {
            dvec2 cursorChange = currentCursor - lastCursor;
            dvec3 currentRight = dvec3(1.0, 0.0, 0) * rotation, currentUp = dvec3(0, 1.0, 0) * rotation;
            rotation *= glm::angleAxis(glm::radians(cursorChange.y), currentRight) * glm::angleAxis(glm::radians(cursorChange.x), currentUp);

            rotateCamera.SetRotation(rotation);
        }
        if (mouseScrollY)
        {
            rotateCamera.SetDistance(rotateCamera.viewDistance * (1.0 - 0.01 * mouseScrollY));
        }
        if (winInfo->resized)
        {
            int fbW, fbH;
            glfwGetFramebufferSize(window, &fbW, &fbH);
            rotateCamera.aspect = double(fbW) / fbH;
            rotateCamera.CreatePerspectiveProjection();
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        linesDisplay.Display(rotateCamera);
        arcsDisplay.Display(rotateCamera);
        axisDisplay.Display(rotateCamera);
        pointDisplay.Display(rotateCamera);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        ImGui::SetNextWindowPos(ImVec2(0, 250), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
        if (ImGui::Begin("Debug", nullptr, window_flags))
        {
            if (ImGui::Button("Button"))
            {
                if (GetOpenFileNameA(&ofn) == TRUE)
                {
                    linesDisplay.Clear();
                    arcsDisplay.Clear();
                    pointDisplay.Clear();

                    std::wcout << szFile << std::endl;
                    dxflib::cadfile dxf(szFile);

                    const std::vector<dxflib::entities::line>& lines = dxf.get_lines();
                    const std::vector<dxflib::entities::lwpolyline>& lwpolylines = dxf.get_lwpolylines();
                    const std::vector<dxflib::entities::polyline>& polylines = dxf.get_polylines();
                    const std::vector<dxflib::entities::hatch>& hatches = dxf.get_hatches();
                    const std::vector<dxflib::entities::arc>& arcs = dxf.get_arcs();
                    const std::vector<dxflib::entities::circle>& circles = dxf.get_circles();
                    const std::vector<dxflib::entities::point_base>& points = dxf.get_points();
                    const std::vector<dxflib::entities::vertex>& vertexs = dxf.get_vertexs();
                    const std::vector<dxflib::entities::solid>& solids = dxf.get_solids();
                    const std::vector<dxflib::entities::solid3d>& solid3ds = dxf.get_solid3ds();
                    const std::vector<dxflib::entities::ellipse>& ellipses = dxf.get_ellipses();
                    const std::vector<dxflib::entities::spline>& splines = dxf.get_splines();

                    std::stringstream ss;
                    for (auto& line : lines)
                    {
                        ss << "- Line (" << line.get_vertex(0).x << ", " << line.get_vertex(0).y
                            << "), (" << line.get_vertex(0).x << ", " << line.get_vertex(1).y << ")\n";
                    }
                    for (auto& lwpolyline : lwpolylines)
                    {
                        auto& lines = lwpolyline.get_lines();
                        if (lines.size() > 0)
                        {
                            ss << "- lwpolyline (" << lines[0][0].x << ", " << lines[0][0].y << ")\n";
                        }
                    }
                    for (auto& polyline : polylines)
                    {
                        auto& lines = polyline.get_lines();
                        if (lines.size() > 0)
                        {
                            ss << "- polyline (" << lines[0][0].x << ", " << lines[0][0].y << ")\n";
                        }
                    }
                    for (auto& circle : circles)
                    {
                        ss << "- Circle (" << circle.get_center_point().x << ", " << circle.get_center_point().y << "), " << circle.get_radius() << "\n";
                    }
                    for (auto& arc : arcs)
                    {
                        ss << "- Arc (" << arc.get_center_point().x << ", " << arc.get_center_point().y << "), " << arc.get_radius()
                            << ", " << arc.get_start_angle() << ", " << arc.get_end_angle() << "\n";
                    }
                    for (auto& point : points)
                    {
                        ss << "- Point (" << point.x << ", " << point.y << ", " << point.z << ")\n";
                    }
                    for (auto& point : vertexs)
                    {
                        ss << "- Vertex (" << point.x << ", " << point.y << ", " << point.z << ")\n";
                    }
                    for (int i = 0; i < solids.size(); i++)
                    {
                        ss << "- Solid - " << i+1 << "\n";
                    }
                    for (int i = 0; i < solid3ds.size(); i++)
                    {
                        ss << "- 3DSolid - " << i+1 << "\n";
                    }
                    for (int i = 0; i < splines.size(); i++)
                    {
                        ss << "- Spline - " << i+1 << "\n";
                    }

                    imageInfo = ss.str();

                    dvec3 min_p{}, max_p{};

                    for (auto& line : lines)
                    {
                        linesDisplay.DrawLine(line);
                        min_p.x = min(line.get_vertex(0).x, min_p.x);
                        min_p.x = min(line.get_vertex(1).x, min_p.x);
                        min_p.y = min(line.get_vertex(0).y, min_p.y);
                        min_p.y = min(line.get_vertex(1).y, min_p.y);

                        max_p.x = max(line.get_vertex(0).x, max_p.x);
                        max_p.x = max(line.get_vertex(1).x, max_p.x);
                        max_p.y = max(line.get_vertex(0).y, max_p.y);
                        max_p.y = max(line.get_vertex(1).y, max_p.y);
                    }

                    for (auto& lwpolyline : lwpolylines)
                    {
                        for (auto& line : lwpolyline.get_lines())
                        {
                            linesDisplay.DrawLine(line);
                            min_p.x = min(line[0].x, min_p.x);
                            min_p.x = min(line[1].x, min_p.x);
                            min_p.y = min(line[0].y, min_p.y);
                            min_p.y = min(line[1].y, min_p.y);

                            max_p.x = max(line[0].x, max_p.x);
                            max_p.x = max(line[1].x, max_p.x);
                            max_p.y = max(line[0].y, max_p.y);
                            max_p.y = max(line[1].y, max_p.y);
                        }
                    }

                    for (auto& polyline : polylines)
                    {
                        for (auto& line : polyline.get_lines())
                        {
                            linesDisplay.DrawLine(line);
                            min_p.x = min(line[0].x, min_p.x);
                            min_p.x = min(line[1].x, min_p.x);
                            min_p.y = min(line[0].y, min_p.y);
                            min_p.y = min(line[1].y, min_p.y);

                            max_p.x = max(line[0].x, max_p.x);
                            max_p.x = max(line[1].x, max_p.x);
                            max_p.y = max(line[0].y, max_p.y);
                            max_p.y = max(line[1].y, max_p.y);
                        }
                    }

                    for (auto& solid : solids)
                    {
                        linesDisplay.DrawLine(solid);
                        min_p.x = min(solid.get_vertex(0).x, min_p.x);
                        min_p.x = min(solid.get_vertex(1).x, min_p.x);
                        min_p.x = min(solid.get_vertex(2).x, min_p.x);
                        min_p.y = min(solid.get_vertex(0).y, min_p.y);
                        min_p.y = min(solid.get_vertex(1).y, min_p.y);
                        min_p.y = min(solid.get_vertex(2).y, min_p.y);

                        max_p.x = max(solid.get_vertex(0).x, max_p.x);
                        max_p.x = max(solid.get_vertex(1).x, max_p.x);
                        max_p.x = max(solid.get_vertex(2).x, max_p.x);
                        max_p.y = max(solid.get_vertex(0).y, max_p.y);
                        max_p.y = max(solid.get_vertex(1).y, max_p.y);
                        max_p.y = max(solid.get_vertex(2).y, max_p.y);
                    }

                    for (auto& hatch : hatches)
                    {
                        for (auto& line : hatch.get_geolines())
                        {
                            linesDisplay.DrawLine(line);
                            min_p.x = min(line[0].x, min_p.x);
                            min_p.x = min(line[1].x, min_p.x);
                            min_p.y = min(line[0].y, min_p.y);
                            min_p.y = min(line[1].y, min_p.y);

                            max_p.x = max(line[0].x, max_p.x);
                            max_p.x = max(line[1].x, max_p.x);
                            max_p.y = max(line[0].y, max_p.y);
                            max_p.y = max(line[1].y, max_p.y);
                        }
                    }

                    for (auto& point : points)
                    {
                        pointDisplay.DrawPoint(point);

                        min_p.x = min(point.x, min_p.x);
                        min_p.y = min(point.y, min_p.y);
                        max_p.x = max(point.x, max_p.x);
                        max_p.y = max(point.y, max_p.y);
                    }

                    linesDisplay.DrawLine(vertexs);
                    for (auto& point : vertexs)
                    {
                        min_p.x = min(point.x, min_p.x);
                        min_p.y = min(point.y, min_p.y);
                        max_p.x = max(point.x, max_p.x);
                        max_p.y = max(point.y, max_p.y);
                    }

                    for (auto& arc : arcs)
                    {
                        arcsDisplay.DrawArc(arc);

                        min_p.x = min(arc.get_center_point().x, min_p.x);
                        min_p.y = min(arc.get_center_point().y, min_p.y);
                        max_p.x = max(arc.get_center_point().x, max_p.x);
                        max_p.y = max(arc.get_center_point().y, max_p.y);
                    }

                    for (auto& circle : circles)
                    {
                        arcsDisplay.DrawArc(circle);

                        min_p.x = min(circle.get_center_point().x, min_p.x);
                        min_p.y = min(circle.get_center_point().y, min_p.y);
                        max_p.x = max(circle.get_center_point().x, max_p.x);
                        max_p.y = max(circle.get_center_point().y, max_p.y);
                    }

                    for (auto& spline : splines)
                    {
                        linesDisplay.DrawLine(spline);

                        const std::vector<dxflib::entities::vertex> control_points = spline.get_control_points();
                        for (int i = 0; i < control_points.size(); i++)
                        {
                            min_p.x = min(control_points[i].x, min_p.x);
                            min_p.y = min(control_points[i].y, min_p.y);
                            max_p.x = max(control_points[i].x, max_p.x);
                            max_p.y = max(control_points[i].y, max_p.y);
                        }
                    }

                    rotateCamera.SetCenter(dvec3(min_p + max_p) * 0.5);
                    viewDistance = glm::length(max_p - min_p);
                    rotateCamera.SetDistance(viewDistance);
                }
            }

            ImGui::End();
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(200, 250));
            if (fileTitle[0]) 
            {
                ImGui::Begin(&fileTitle[0], nullptr, window_flags);
            }
            else
            {
                ImGui::Begin("Image", nullptr, window_flags);
            }
            ImGui::TextWrapped(imageInfo.c_str());
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        winInfo->NewFrameState();
        NewFrameInputStates();
        glfwPollEvents();

    }
}
