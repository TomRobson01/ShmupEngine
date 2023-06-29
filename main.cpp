#include <iostream>
#include <vector>

#ifdef TRGAME
#include "TRInput.h"
#include "TRLogger.h"
#include "Gameplay/AI/TREnemy.h"
#include "Objects/TRPlayer.h"
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
	std::cout << "Hello, game!";

	// Initialize our renderer and OpenGL by calling QInstance once.
	GLFWwindow* window = TRRenderer::QInstance().QWindow();

	// Load the world
	// --------------------------

	// We always instantiate our player via code, as they're a given. However, we'll do this via a TRWorld method later
	TRWorld::QInstance()->InstanciateObject<TRPlayer>(TRWorld::QInstance()->QObjPlayer(), Transform(0, 0, 0, 0), 1.0f, CollisionLayer::CL_PLAYER);

	for (int i = 0; i < 1; i++)
		TRWorld::QInstance()->InstanciateObject<TREnemy>(TRWorld::QInstance()->QObjEnemy(), Transform(5, 0, 0, 0), 1.0f, CollisionLayer::CL_ENEMY);


	// Wireframe mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FLAT);	// GL_FLAT = normal
												// GL_LINE = wireframe

	// Initialize input system
	TRInput::QInstance()->Init(window);

	TRPhysics::QInstance()->InitCollisionThread();

	// Primary game loop!
	while (!glfwWindowShouldClose(window))		// For the sake of keeping things seperate, this won't be running on OpenGL for long
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