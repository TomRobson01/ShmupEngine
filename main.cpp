#include <iostream>
#include <vector>

#ifdef TRGAME
#include "TRInput.h"
#include "TRLogger.h"
#include "Objects/TRPlayer.h"
#include "Objects/TRWorld.h"
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

	// Initialize our renderer and OpenGL by calling QInstance once.
	TRRenderer::QInstance();
	GLFWwindow* window = TRRenderer::QInstance().QWindow();

	// Load the world
	// --------------------------

	// First, we forward load all of our world objects that we may need for the given level - this will be handled from TRWorld once we have serialization
	TRObject ObjBricks("Brick", "Assets/Textures/T_Bricks.png");
	TRObject ObjBricksIvy("Brick Ivy", "Assets/Textures/T_Bricks_Ivy.png");
	TRObject ObjPlayer("Player", "Assets/Textures/T_Temp_Player_Sprite.png");

	// We always instantiate our player via code, as they're a given. However, we'll do this via a TRWorld method later
	TRWorld::QInstance()->InstanciateObject<TRPlayer>(ObjPlayer, Transform(0, 0, 0, 0));


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
#endif
	return 1;
}