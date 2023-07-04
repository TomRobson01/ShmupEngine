#include <iostream>
#include <vector>

#ifdef TRGAME
#include "TRInput.h"
#include "TRLogger.h"
#include "Gameplay/AI/TREnemy.h"
#include "Objects/TRWorld.h"
#include "Rendering/TextureLoader.h"
#endif

#ifdef TREDITOR
int main()
{
	return 0;
}
#endif // TREDITOR


#ifdef TRGAME
// GLFW input handler
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
int main()
{
	// Initialize our renderer and OpenGL by calling QInstance once.
	GLFWwindow* window = TRRenderer::QInstance().QWindow();

	// Load the world
	// --------------------------

	// Wireframe mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FLAT);	// GL_FLAT = normal
												// GL_LINE = wireframe

	// Initialize input system
	TRInput::QInstance()->Init(window);

	TRPhysics::QInstance()->InitCollisionThread();

	// Primary game loop!
	while (!glfwWindowShouldClose(window) && !TRWorld::QInstance()->QShouldQuit())
	{
		// Process input
		TRInput::QInstance()->PollInputs();

		TRWorld::QInstance()->UpdateWorld();
		TRRenderer::QInstance().RenderStack();
	}

	TRPhysics::QInstance()->ShutdownCollisionThread();

	TRWorld::QInstance()->UnloadWorld();
	TRRenderer::QInstance().Shutdown();
	return 1;
}
#endif