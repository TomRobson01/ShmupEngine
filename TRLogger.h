#pragma once

#define TRASSERT(abPassConditions, asAssertText) (TRLogger::Assert(abPassConditions, asAssertText))
#define TRDEVASSERT(abPassConditions, asAssertText) (TRLogger::Assert(abPassConditions, asAssertText, "ASSERT - DEV", true))


#include <string>
#include <Windows.h>

enum class LogSeverity
{
	TR_DEFAULT,
	TR_WARNING,
	TR_ERROR,
	TR_FATAL,
	TR_COUNT
};

class TRLogger
{
public:
	static void Assert(bool abPassConditions, const char* asAssertText, const char* asTitle = "ASSERT", bool abDevAssert = false) 
	{ 
		if (!abDevAssert && !abPassConditions)
		{
			MessageBoxA(NULL, asAssertText, asTitle, MB_OK | MB_ICONERROR);
		}
		else if (!abPassConditions) 
		{ 
#ifdef _DEBUG
			MessageBoxA(NULL, asAssertText, asTitle, MB_OK | MB_ICONERROR);
#endif // _DEBUG
		} 
	};

#ifdef TRGAME
	// For now, locking log to game only with imgui. Can investigate editor logs later.
	void Log(const char* asLog, LogSeverity aeSeverity = LogSeverity::TR_DEFAULT);
#endif

	static TRLogger* QInstance();


private:
	TRLogger() {}

	static TRLogger* instancePtr;
};


