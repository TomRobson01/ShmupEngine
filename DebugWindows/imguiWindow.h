#pragma once

#include "ImGuiDefines.h"

class imguiWindow
{
public:
	void InitWindow(const char* asName, ImVec2 avStartPos, ImVec2 avScale, int aiFlags);

	virtual void Draw();

	const ImVec2 QPos() { return startPos; }
	const ImVec2 QSize() { return windowSize; }

protected:

	const char* sName = "Window";
	ImVec2 startPos;
	ImVec2 windowSize = ImVec2(256, 256);

	int flags = ImGuiWindowFlags_None;
};

