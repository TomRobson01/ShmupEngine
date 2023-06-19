#include "ObjectView.h"
#include "EditorDefines.h"

#include <windows.h>


namespace 
{
	HWND hwnd;
}

ObjectView* ObjectView::instancePtr;
ObjectView* ObjectView::QInstance()
{
	if (instancePtr == nullptr)
	{
		instancePtr = new ObjectView();
	}
	return instancePtr;
}

/// <summary>
/// Creates the necessary windows for the Object View. This includes the header text, filter, border, and ListBox.
/// </summary>
/// <param name="ahwnd">The parent HWND for the application.</param>
void ObjectView::Init(HWND ahwnd)
{
	CreateWindowW(L"static", L"", WS_VISIBLE | WS_BORDER | WS_CHILD, 5, 16, 310, 620, ahwnd, NULL, NULL, NULL);

	CreateWindowW(L"static", L"    Object List", WS_VISIBLE | WS_CHILD, 10, 10, 100, 50, ahwnd, NULL, NULL, NULL);

	CreateWindowW(L"static", L"Filter", WS_VISIBLE | WS_CHILD, 20, 40, 40, 50, ahwnd, NULL, NULL, NULL);
	CreateWindowW(L"Edit", L"...", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOHSCROLL, 60, 40, 240, 20, ahwnd, (HMENU)ID_OBJLISTVIEW_FILTER, NULL, NULL);

	hwnd = CreateWindow(L"LISTBOX",
		L"ObjectListBox",
		WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_STANDARD | LBS_HASSTRINGS,
		10, 80,
		300,
		560,
		ahwnd,
		(HMENU)ID_OBJLISTVIEW,
		NULL,
		NULL);
}

/// <summary>
/// Adds an object to the data set the ObjectView will view.
/// </summary>
/// <param name="asName">The name this data will be displayed in the View as. Note: This cannot be the same as another item, and will display a warning.</param>
void ObjectView::AddObject(LPCWSTR asName)
{
	SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM)asName);
	SetFocus(hwnd);
}

/// <summary>
/// Returns the HWND for the ListBox.
/// </summary>
/// <returns>The HWND for the ListBox of the ObjectView.</returns>
HWND ObjectView::QHWND()
{
	return hwnd;
}


