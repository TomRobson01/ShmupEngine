#pragma once

#include <windows.h>
#include "EditorDefines.h"

class EditorGUI
{
public:
	static EditorGUI* QInstance();
	void Init(HWND aHwnd);

#ifdef _DEBUG
	void SendDebugListMessage();
#endif // _DEBUG


protected:
	void AddMenus();
	void AddControls();

private:
	EditorGUI() {}

	static EditorGUI* instancePtr;

	HWND hwnd;

	// Menus
	HMENU hMainMenu;
};

