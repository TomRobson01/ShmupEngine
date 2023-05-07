#include <iostream>
#include <vector>

#ifdef TRGAME
#include "TRLogger.h"
#include "Objects/TRObject.h"
#include "Rendering/TRRenderer.h"
#include "Rendering/TextureLoader.h"
#endif



#ifdef TRGAME
// GLFW input handler
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
#endif

int main()
{
#ifdef TREDITOR
	std::cout << "Hello, editor!";
#endif

#ifdef TRGAME
	std::cout << "Hello, game!";
	// OPENGL Setup
	// Migrate all this to its own class

	// Initialize our renderer and OpenGL by calling QInstance once.
	TRRenderer::QInstance();
	GLFWwindow* window = TRRenderer::QInstance().QWindow();

	// Load and create a texture
	// --------------------------
	std::vector<TRObject*> worldObjects;	// We probably want to move this over to a "World" class
	worldObjects.push_back(new TRObject("Brick Ivy", "Assets/Textures/T_Bricks_Ivy.png", Transform(0, 0, 0, 20)));
	worldObjects.push_back(new TRObject("Brick", "Assets/Textures/T_Bricks.png", Transform(1, 0, 0, 0)));


	// Wireframe mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FLAT);	// GL_FLAT = normal
												// GL_LINE = wireframe
	
	TRDEVASSERT(true, "Quick assert!");
	TRLogger::QInstance()->Log("Quick log", LogSeverity::TR_DEFAULT);
	TRLogger::QInstance()->Log("Quick log", LogSeverity::TR_WARNING);
	TRLogger::QInstance()->Log("Quick log", LogSeverity::TR_ERROR);
	TRLogger::QInstance()->Log("Quick log", LogSeverity::TR_FATAL);

	// Primary game loop!
	while (!glfwWindowShouldClose(window))		// For the sake of keeping things seperate, this won't be running on OpenGL for long
	{

		// Process input
		processInput(window);

		// Any gameplay calculations
		// Update all world objects
		for (TRObject* obj : worldObjects)
		{
			obj->Update();

			// Once everything has been processed, add them to the renderer
			Transform* t = obj->QTransform();
			TRRenderer::QInstance().AddRenderTarget(
				t->QPositionX(),
				t->QPositionY(),
				t->QPositionZ(),
				t->QRotation(),
				obj->QTexture());
		}

		TRRenderer::QInstance().RenderStack();
	}

	// Delete our world objects - again, probably wants to be in a generic World class
	for (TRObject* obj : worldObjects)
	{
		delete obj;
	}

	TRRenderer::QInstance().Shutdown();
#endif

	return 1;
}