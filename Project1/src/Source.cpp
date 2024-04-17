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
#include <functional>
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
	
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetScrollCallback(window, MouseScrollCallback);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

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
	LinesDisplay linesDisplay;
	ArcsDisplay arcsDisplay;

	RotateCamera rotateCamera;

	double viewDistance = 5.0;
	glm::dquat rotation(dvec3(0.0, 0.0, 0.0));
	float dist = 5.0f;
	rotateCamera.projMat = glm::perspective(45.0, 4.0 / 3.0, 0.0001, 10000.0);
	rotateCamera.rotMat = glm::toMat4(rotation);
	rotateCamera.postRotTransMat = glm::translate(dvec3(0, 0, -5.0));
	rotateCamera.preRotTransMat = glm::translate(dvec3(0));
	rotateCamera.SetDistance(viewDistance);

	HWND win32window = glfwGetWin32Window(window);
	OPENFILENAMEA ofn;
	CHAR szFile[MAX_PATH] = { 0 };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "(*.dxf)\0*.dxf\0";

	ofn.nFilterIndex = 1;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = "Select a File";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING;



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

		glClear(GL_COLOR_BUFFER_BIT);


		linesDisplay.Display(rotateCamera);
		arcsDisplay.Display(rotateCamera);


		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
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

					std::wcout << szFile << std::endl;
					dxflib::cadfile test(szFile);

					std::vector<dxflib::entities::line>& lines = test.get_lines();
					const std::vector<dxflib::entities::arc>& arcs = test.get_arcs();
					const std::vector<dxflib::entities::circle>& circles = test.get_circles();

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
					rotateCamera.SetCenter(dvec3(min_p + max_p) * 0.5);
					viewDistance = glm::length(max_p - min_p);
					rotateCamera.SetDistance(viewDistance);

				}
			}

			ImGui::End();
		}

		//if (ImGui::DragFloat("dist", &dist))
		//{
		//	linesDisplay.SetDistance((double)dist);
		//}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);

		NewFrameInputStates();
		glfwPollEvents();
	}
}
