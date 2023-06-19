#pragma once

#include <windows.h>

class ObjectView
{
public:
	static ObjectView* QInstance();
	void Init(HWND ahwnd);
	void AddObject(LPCWSTR asName);

	HWND QHWND();

private:
	ObjectView() {}

	static ObjectView* instancePtr;
};

