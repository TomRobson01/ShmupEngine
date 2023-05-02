#pragma once

#include "Libraries/imGUI/imgui.h"
#include "Libraries/imGUI/imgui_impl_glfw.h"
#include "Libraries/imGUI/imgui_impl_opengl3.h"

class imguiWindow
{
public:
	void CreateWindow(const char* asName, ImVec2 avStartPos, ImVec2 avScale, int aiFlags);

	virtual void Draw();

protected:

	const char* sName = "Window";
	ImVec2 startPos;
	ImVec2 windowSize = ImVec2(256, 256);

	int flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;
};

