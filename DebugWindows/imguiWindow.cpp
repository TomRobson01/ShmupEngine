#include "imguiWindow.h"

void imguiWindow::CreateWindow(const char* asName, ImVec2 avStartPos, ImVec2 avScale, int aiFlags)
{
	sName = asName;
	startPos = avStartPos;
	windowSize = avScale;
	flags = aiFlags;
}

void imguiWindow::Draw()
{
	if (ImGui::Begin(sName, NULL, flags))
	{
	}
	ImGui::End();
}
