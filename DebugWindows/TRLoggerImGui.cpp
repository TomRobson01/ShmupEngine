#include "TRLoggerImGui.h"

void TRLoggerImGui::Draw()
{
	if (ImGui::Begin(sName, NULL, flags))
	{
		ImGui::TextColored(IMGUICOL_GREEN, "Debug Log");
		ImGui::Separator();
		ImGui::Separator();

		ImGui::Columns(2, nullptr, false);


		for (LogEntry entry : log)
		{

			ImVec4 col = IMGUICOL_GREY;
			const char* severityName = "Log";
			switch (entry.eSeverity)
			{
			case LogSeverity::TR_WARNING:
				col = IMGUICOL_YELLOW;
				severityName = "Warning";
				break;
			case LogSeverity::TR_ERROR:
				col = IMGUICOL_RED;
				severityName = "Error";
				break;
			case LogSeverity::TR_FATAL:
				col = IMGUICOL_RED_DEEP;
				severityName = "FATAL";
				break;
			}

			// Severity name
			ImGui::TextColored(col, severityName);
			ImGui::NextColumn();

			// Considering putting the origin here - need to look into templates some more

			// Axtual text
			ImGui::TextColored(col, entry.sText);
			ImGui::NextColumn();

			ImGui::Separator();
		}
	}
	ImGui::End();
}

TRLoggerImGui* TRLoggerImGui::instancePtr;
TRLoggerImGui* TRLoggerImGui::QInstance()
{
	if (instancePtr == nullptr)
	{
		instancePtr = new TRLoggerImGui();
	}
	return instancePtr;
}