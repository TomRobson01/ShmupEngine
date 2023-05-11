#include "TRInput.h"

#include <vector>
#include <GLFW/glfw3.h>


namespace
{
	struct MouseDownDatum
	{
		bool bMousePressed;
		bool bMouseReleased;
		bool bMouseDown;
	};

	// Vectors maintaining keys currently pressed, and the ones pressed or released on the current frame
	std::vector<TRInputKey> vPressedInputKeys;
	std::vector<TRInputKey> vReleasedInputKeys;
	std::vector<TRInputKey> vCurrentInputKeys;

	MouseDownDatum LeftMouseStatus;
	MouseDownDatum RightMouseStatus;

	void AddPressedKey(TRInputKey aKey)
	{
		vPressedInputKeys.push_back(aKey);
		vCurrentInputKeys.push_back(aKey);
	}
}


TRInput* TRInput::instancePtr;
TRInput* TRInput::QInstance()
{
	if (instancePtr == nullptr)
	{
		instancePtr = new TRInput();
	}
	return instancePtr;
}

void TRInput::Init(GLFWwindow* apWindow)
{
	window = apWindow;
}

///<summary> Polls all inputs for this frame, and caches the values for appropriate access from elsewhere in the application. </summary>
void TRInput::PollInputs()
{
	// KEYS --------
	vPressedInputKeys.clear();
	vReleasedInputKeys.clear();

	// Check for any pressed keys
	auto KeyCheckFunctor = [this](TRInputKey aKey, GLFWwindow* apWindow)
	{
		if (!QKey(aKey) && glfwGetKey(apWindow, aKey) == GLFW_PRESS)
		{
			AddPressedKey(aKey);
		}
	};

	// !!! ADD NEW TRInputKeys HERE !!!
	KeyCheckFunctor(TRInputKey::UP,		window);
	KeyCheckFunctor(TRInputKey::DOWN,	window);
	KeyCheckFunctor(TRInputKey::LEFT,	window);
	KeyCheckFunctor(TRInputKey::RIGHT,	window);


	// Check for released keys
	for (TRInputKey k : vCurrentInputKeys)
	{
		if (glfwGetKey(window, k) == GLFW_RELEASE)
		{
			vCurrentInputKeys.erase(std::remove(vCurrentInputKeys.begin(), vCurrentInputKeys.end(), k));
			vReleasedInputKeys.push_back(k);
		}
	}

	// MOUSE --------
	auto MouseChecker = [this](MouseDownDatum* aData, int aiMouseTarget, GLFWwindow* apWindow)
	{
		aData->bMousePressed = false;
		aData->bMouseReleased = false;

		bool bIsMouseDown = glfwGetMouseButton(apWindow, aiMouseTarget) == GLFW_PRESS;
		if (bIsMouseDown && !aData->bMouseDown)
		{
			aData->bMousePressed = true;
		}
		else if (!bIsMouseDown && aData->bMouseDown)
		{
			aData->bMouseReleased = true;
		}

		aData->bMouseDown = bIsMouseDown;
	};

	MouseChecker(&LeftMouseStatus, TRMOUSE_LEFT, window);
	MouseChecker(&RightMouseStatus, TRMOUSE_RIGHT, window);

	glfwGetCursorPos(window, &mouseXPos, &mouseYPos);
}

///<summary> Checks if the given key is within the recently pressed keys list.</summary>
///<param=aeTargetKey> The key we want to query. </param>
bool TRInput::QKeyPressed(TRInputKey aeTargetKey)
{
	return std::find(vPressedInputKeys.begin(), vPressedInputKeys.end(), aeTargetKey) != vPressedInputKeys.end();
}

///<summary> Checks if the given key is within the currently pressed keys list.</summary>
///<param=aeTargetKey> The key we want to query. </param>
bool TRInput::QKey(TRInputKey aeTargetKey)
{
	return std::find(vCurrentInputKeys.begin(), vCurrentInputKeys.end(), aeTargetKey) != vCurrentInputKeys.end();
}

///<summary> Checks if the given key is within the recently released keys list.</summary>
///<param=aeTargetKey> The key we want to query. </param>
bool TRInput::QKeyReleased(TRInputKey aeTargetKey)
{
	return std::find(vReleasedInputKeys.begin(), vReleasedInputKeys.end(), aeTargetKey) != vReleasedInputKeys.end();
}

///<summary> Checks the given mouse buttons datum to see if it was pressed this frame.</summary>
///<param=aeTargetKey> ID of the mouse button we wish to query. Use TRMOUSE_LEFT or TRMOUSE_RIGHT </param>
bool TRInput::QMousePressed(int aiMouseTarget)
{
	bool bRetVal = false;
	switch (aiMouseTarget)
	{
	case TRMOUSE_LEFT:
		bRetVal = LeftMouseStatus.bMousePressed;
		break;
	case TRMOUSE_RIGHT:
		bRetVal = RightMouseStatus.bMousePressed;
		break;
	}
	return bRetVal;
}

///<summary> Checks the given mouse buttons datum to see if is currently held.</summary>
///<param=aeTargetKey> ID of the mouse button we wish to query. Use TRMOUSE_LEFT or TRMOUSE_RIGHT </param>
bool TRInput::QMouseHeld(int aiMouseTarget)
{
	bool bRetVal = false;
	switch (aiMouseTarget)
	{
	case TRMOUSE_LEFT:
		bRetVal = LeftMouseStatus.bMouseDown;
		break;
	case TRMOUSE_RIGHT:
		bRetVal = RightMouseStatus.bMouseDown;
		break;
	}

	return bRetVal;
}

///<summary> Checks the given mouse buttons datum to see if it was released this frame.</summary>
///<param=aeTargetKey> ID of the mouse button we wish to query. Use TRMOUSE_LEFT or TRMOUSE_RIGHT </param>
bool TRInput::QMouseReleased(int aiMouseTarget)
{
	bool bRetVal = false;
	switch (aiMouseTarget)
	{
	case TRMOUSE_LEFT:
		bRetVal = LeftMouseStatus.bMouseReleased;
		break;
	case TRMOUSE_RIGHT:
		bRetVal = RightMouseStatus.bMouseReleased;
		break;
	}
	return bRetVal;
}

