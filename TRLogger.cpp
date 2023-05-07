#include "TRLogger.h"
#include "DebugWindows/TRLoggerImGui.h"

TRLogger* TRLogger::instancePtr;
TRLogger* TRLogger::QInstance()
{
	if (instancePtr == nullptr)
	{
		instancePtr = new TRLogger();
	}
	return instancePtr;
}

#ifdef TRGAME
void TRLogger::Log(const char* asLog, LogSeverity aeSeverity)
{
#ifdef _DEBUG
	TRLoggerImGui::QInstance()->AddLog(asLog, aeSeverity);
#endif
}
#endif