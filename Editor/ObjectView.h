#pragma once

#include <windows.h>

class ObjectView
{
public:
	static ObjectView* QInstance();
	void Init(HWND ahwnd);
	void AddObject(LPCWSTR asName);

	void OnFilterChanged();

	HWND QHWND();

protected:
	void DrawListBox();

private:
	ObjectView() {}

	static ObjectView* instancePtr;
};

