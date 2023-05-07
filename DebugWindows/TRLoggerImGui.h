#pragma once

#include <Vector>

#include "TRLogger.h"
#include "DebugWindows/imguiWindow.h"

struct LogEntry 
{
public:
	LogEntry()
	{
		sText		= "Null Log";
		eSeverity	= LogSeverity::TR_ERROR;
	}

	LogEntry(const char* asNewLog, LogSeverity aeSeverity)
	{
		sText = asNewLog;
		eSeverity = aeSeverity;
	}


	const char* sOrigin;
	const char* sText;
	LogSeverity eSeverity;
};

class TRLoggerImGui : public imguiWindow
{
public:
	// Singleton Functions
	TRLoggerImGui()
	{
		instancePtr = this;
	}
	static TRLoggerImGui* QInstance();

	// Overrides
	virtual void Draw() override;

	// Logging
	void AddLog(const char* asNewLog, LogSeverity aeSeverity) { this->log.push_back(LogEntry(asNewLog, aeSeverity)); }


private:
	static TRLoggerImGui* instancePtr;
	std::vector<LogEntry> log;

};

