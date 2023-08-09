#pragma once

#include "DebugWindows/imguiWindow.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

struct TRRenderTarget
{
public:
	TRRenderTarget() = default;
	TRRenderTarget(float aiX, float aiY, float aiZ, float afRotation, unsigned int aiTexture, float afScale = 1.0f, bool abScrolls = false)
	{
		position = glm::vec3(aiX, aiY, aiZ);
		rotation = afRotation;
		texture = aiTexture;
		scale = afScale;
		scrolls = abScrolls;
	};


	unsigned int const	QTexture()		{ return texture; }
	glm::vec3 const		QPosition()		{ return position; }
	float const			QRotation()		{ return rotation; }
	float const			QScale()		{ return scale; }
	bool const			QScrolls()		{ return scrolls; }

private:
	unsigned int texture;

	glm::vec3 position;
	float rotation;
	float scale;
	bool scrolls;
};

class TRRenderer
{
public:
	static TRRenderer QInstance()
	{
		static TRRenderer r;
		return r;
	};

	// Constructor handles initialization of OpenGL
	TRRenderer();

	void Shutdown();

	void AddRenderTarget(float aiX, float aiY, float aiZ, float afRotation, unsigned int aiTexture, float afScale = 1.0f, bool abScrolls = false);
	void RenderStack();

	void SetCameraShake(bool abEnabled);

#ifdef _DEBUG
	void AddImguiWindowToRender(std::shared_ptr<imguiWindow> apnewWindow) { imguiWindowStack.push_back(apnewWindow); };
#endif

	GLFWwindow* QWindow() { return window; };

private:

	GLFWwindow* window;

	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;

	int shaderProgram;

	// Create vertex data
	float verticies[36] =
	{
		// Points				// Colors				// Texture coords
		-1.0f,  1.0f, 0.0f,		1.0f, 1.0f, 1.0f,		0.0f, 0.0f,			// Top Left
		 1.0f,  1.0f, 0.0f,		1.0f, 1.0f, 1.0f,		1.0f, 0.0f,			// Top Right
		-1.0f, -1.0f, 0.0f,		1.0f, 1.0f, 1.0f,		0.0f, 1.0f,			// Bottom Left
		 1.0f, -1.0f, 0.0f,		1.0f, 1.0f, 1.0f,		1.0f, 1.0f			// Bottom Right
	};

	unsigned int indices[6] = {
		0, 1, 3,	// First tri
		3, 2, 0		// Second tri
	};

	std::vector<std::shared_ptr<imguiWindow>> imguiWindowStack;
};

