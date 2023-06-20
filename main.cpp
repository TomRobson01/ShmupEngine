#include <iostream>
#include <vector>

#ifdef TRGAME
#include "TRInput.h"
#include "TRLogger.h"
#include "Gameplay/AI/TREnemy.h"
#include "Objects/TRPlayer.h"
#include "Objects/TRWorld.h"
#include "Rendering/TextureLoader.h"
#elif TREDITOR
#include "Editor/EditorGUI.h"
#include "Editor/ObjectView.h"
#endif


#ifdef TREDITOR

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_CREATE:
		EditorGUI::QInstance()->Init(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_COMMAND:
		switch (wp)
		{
		case MSG_FILE_NEW:
			MessageBeep(MB_OK);
			break;
#ifdef _DEBUG
		case MSG_DEBUG_OBJVIEW_ADDTEST:
			EditorGUI::QInstance()->SendDebugListMessage();
			break;
#endif
		default:
			break;
		}

		// -------- HIWORD --------
		switch (HIWORD(wp))
		{
		case EN_CHANGE:
			switch (LOWORD(wp))
			{
			case ID_OBJLISTVIEW_FILTER:
				ObjectView::QInstance()->OnFilterChanged();
				break;
			default:
				break;
				// End of EN_CHANGE - LOWORD
			}
			break;
		default:
			break;
			// End of HIWORD
		}
		break;	// End of WM_COMMAND

	default:
		return DefWindowProcW(hwnd, msg, wp, lp);
	}
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
	WNDCLASSW wc = {0};

	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInst;
	wc.lpszClassName = L"WindowClass";
	wc.lpfnWndProc = WindowProcedure;

	if (!RegisterClassW(&wc))
	{
		MessageBox(NULL, L"Failed to register the window class", L"FATAL ERROR ON STARTUP", MB_ICONERROR);
		return -1;
	}

	CreateWindowW(L"WindowClass", L"My Window", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 256, 256, 1280, 720, NULL, NULL, NULL, NULL);

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return 0;
}
#endif


#ifdef TRGAME
// GLFW input handler
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
int main()
{
	std::cout << "Hello, game!";

	// Initialize our renderer and OpenGL by calling QInstance once.
	TRRenderer::QInstance();
	GLFWwindow* window = TRRenderer::QInstance().QWindow();

	// Load the world
	// --------------------------

	// First, we forward load all of our world objects that we may need for the given level - this will be handled from TRWorld once we have serialization
	TRObject ObjBricks("Brick", "Assets/Textures/T_Bricks.png");
	TRObject ObjBricksIvy("Brick Ivy", "Assets/Textures/T_Bricks_Ivy.png");
	TRObject ObjPlayer("Player", "Assets/Textures/T_Temp_Player_Sprite.png");
	TRObject ObjEnemy("Enemy", "Assets/Textures/T_Temp_Player_Sprite.png");

	// We always instantiate our player via code, as they're a given. However, we'll do this via a TRWorld method later
	TRWorld::QInstance()->InstanciateObject<TRPlayer>(ObjPlayer, Transform(0, 0, 0, 0), 1.0f, CollisionLayer::CL_PLAYER);

	TRWorld::QInstance()->InstanciateObject<TREnemy>(ObjEnemy, Transform(10, 0, 0, 0), 1.0f, CollisionLayer::CL_ENEMY);


	// Wireframe mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FLAT);	// GL_FLAT = normal
												// GL_LINE = wireframe

	// Initialize input system
	TRInput::QInstance()->Init(window);


	// Primary game loop!
	while (!glfwWindowShouldClose(window))		// For the sake of keeping things seperate, this won't be running on OpenGL for long
	{

		// Process input
		TRInput::QInstance()->PollInputs();

		TRWorld::QInstance()->UpdateWorld();
		TRRenderer::QInstance().RenderStack();
	}

	TRWorld::QInstance()->UnloadWorld();
	TRRenderer::QInstance().Shutdown();
	return 1;
}
#endif