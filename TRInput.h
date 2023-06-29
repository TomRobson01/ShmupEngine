#pragma once

struct GLFWwindow;
//struct ImVec2;

#define TRMOUSE_LEFT	0
#define TRMOUSE_RIGHT	1

// Check what the keybind defines extend to in glfw3.h for unlisted keys
/* Some common examples:

	Up = 265
	Down = 264
	Left = 263
	Right = 262

	W = 87
	A = 65
	S = 83
	D = 68

	E = 69

	Esc = 256
	Space = 32

*/
// IMPORTANT: If you're adding to this enum, please esnure TRInput::PollInput is aware of it
enum TRInputKey
{
	UP		= 87,
	DOWN	= 83,
	LEFT	= 65,
	RIGHT	= 68,
};

class TRInput
{
public:
	static TRInput* const QInstance();

	void Init(GLFWwindow* apWindow);
	void PollInputs();

	bool const QKeyPressed(TRInputKey aeTargetKey);
	bool const QKey(TRInputKey aeTargetKey);
	bool const QKeyReleased(TRInputKey aeTargetKey);

	bool const QMousePressed(int aiMouseTarget);
	bool const QMouseHeld(int aiMouseTarget);
	bool const QMouseReleased(int aiMouseTarget);

	double const QMousePosX() { return mouseXPos; };
	double const QMousePosY() { return mouseYPos; };

private:
	TRInput() {}

	static TRInput* instancePtr;

	GLFWwindow* window;

	double mouseXPos;
	double mouseYPos;
};

