#include "TestImGUIWindow.h"

void TestImGUIWindow::Draw()
{
	if (ImGui::Begin(sName, NULL, flags))
	{
		ImGui::Text("Hello, world!");
	}
	ImGui::End();
}