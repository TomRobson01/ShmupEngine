#include "EditorGUI.h"
#include "ObjectView.h"

#include <commctrl.h>
#include <string>

EditorGUI* EditorGUI::instancePtr;
EditorGUI* EditorGUI::QInstance()
{
	if (instancePtr == nullptr)
	{
		instancePtr = new EditorGUI();
	}
	return instancePtr;
}

/// <summary>
/// Initialize the editor GUI. Store the main HWND, and add any menus and controls.
/// </summary>
/// <param name="aHwnd">The parent window for the application.</param>
void EditorGUI::Init(HWND aHwnd)
{
	hwnd = aHwnd;

	AddMenus();
	AddControls();
}

#ifdef _DEBUG
/// <summary>
/// Adds some simple test strings to the ObjectView ListBox.
/// </summary>
void EditorGUI::SendDebugListMessage()
{
	ObjectView::QInstance()->AddObject(L"Hello");
	ObjectView::QInstance()->AddObject(L"World");
	ObjectView::QInstance()->AddObject(L"How");
	ObjectView::QInstance()->AddObject(L"Are");
	ObjectView::QInstance()->AddObject(L"You?");
}
#endif // _DEBUG

/// <summary>
/// Add all menus we wish to display at the top of the application. E.G: File, Edit, View, Help, etc.
/// </summary>
void EditorGUI::AddMenus()
{
	hMainMenu = CreateMenu();
	HMENU hFileMenu = CreateMenu();

	// Create everything on the toolbar ------------
	AppendMenuW(hMainMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
#ifdef _DEBUG
	HMENU hDebugMenu = CreateMenu();
	AppendMenuW(hMainMenu, MF_POPUP, (UINT_PTR)hDebugMenu, L"Debug");
#endif
	// Add to the file menu
	AppendMenuW(hFileMenu, MF_STRING, MSG_FILE_NEW, L"New");

	// Populate the menus -----------
#ifdef _DEBUG
	// Populate the debug menu
	AppendMenuW(hDebugMenu, MF_STRING, MSG_DEBUG_OBJVIEW_ADDTEST, L"Add to ListBox");
#endif

	SetMenu(hwnd, hMainMenu);
}

/// <summary>
/// Add any controls to the GUI. This defines most of the GUI, including the ObjectView and the Inspector.
/// </summary>
void EditorGUI::AddControls()
{
	ObjectView::QInstance()->Init(hwnd);

}
