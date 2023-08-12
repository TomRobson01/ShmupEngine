#include <algorithm>
#include <iostream>
#include <vector>

#include "DebugWindows/TRLoggerImGui.h"
#include "DebugWindows/TestImGUIWindow.h"

#include "TRRenderer.h"
#include "TextureLoader.h"
#include "Rendering/ShaderManagement.h"

#include "Gameplay/AI/TRWaveManager.h"

std::vector<TRRenderTarget> RenderTargetStack;
std::atomic<bool> RenderThreadLock;

namespace
{
	int screenWidth		= 800;
	int screenHeight	= 600;

	float fCamZoom = 4.0f;

	float camShakeX = 0;
	float camShakeY = 0;

	float fTime = 0.0f;
	float fScrollSpeed = 1.0f;

	bool bCamShakeEnabled = false;
}

// OpenGL resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	screenWidth		= width;
	screenHeight	= height;
	std::cout << "New Resolution:\n" << screenWidth << ", " << screenHeight << std::endl;
	glViewport(0, 0, screenWidth, screenHeight);
}

TRRenderer::TRRenderer()
{
	std::srand((unsigned)time(0));

	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create our glfw window
	window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

#ifdef _DEBUG
	// Initialize imGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
#endif

	// Create our viewport
	glViewport(0, 0, screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Initialize any of our main classes
	ShaderManagement TRShaderManager;

	// Gen our shader programs that we intend to use for the rest of the application
	shaderProgram = TRShaderManager.GenerateShaderProgram();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	// Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

	// Bind buffer attributes
	// ------------------------
	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Tex Coord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Bind EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// --------------------------

	// Sprite background blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef _DEBUG
	// ImGui window definitions
	std::shared_ptr<imguiWindow> loggerWindow =  std::make_shared<TRLoggerImGui>();
	loggerWindow->InitWindow("Logger", ImVec2(0,0), ImVec2(256, 256), ImGuiWindowFlags_None);
	AddImguiWindowToRender(loggerWindow);
#endif
}

void TRRenderer::Shutdown()
{
	// OpenGL cleanup!
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();
}

void TRRenderer::AddRenderTarget(float aiX, float aiY, float aiZ, float afRotation, unsigned int aiTexture, float afScale, bool abScrolls)
{
	RenderTargetStack.push_back(TRRenderTarget(aiX, aiY, aiZ, afRotation, aiTexture, afScale, abScrolls));
}

void TRRenderer::RenderStack()
{
	// Render to buffer
	glClearColor(0.025f, 0.03f, 0.06f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

#ifdef _DEBUG
	// Prep ImGUI
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
#endif

	// Calculate camera shake
	if (bCamShakeEnabled)
	{
		int iCamShakeX = (rand() % 100);
		int iCamShakeY = (rand() % 100);
		camShakeX = iCamShakeX * 0.001f;
		camShakeY = iCamShakeY * 0.001f;
}
	else
	{
		camShakeX = 0;
		camShakeY = 0;
	}

	// All this needs to happen for each render target
	for (TRRenderTarget target : RenderTargetStack)
	{
		// Bind the texture we want to use
		glBindTexture(GL_TEXTURE_2D, target.QTexture());

		// Transformation code 
		// -----------------------
		// Create transformations
		glm::vec3 pos = target.QPosition();
		pos.x += camShakeX;
		pos.y += camShakeX;
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos);		// Creates initial transformation
		transform = glm::rotate(transform, target.QRotation(), glm::vec3(0.0f, 0.0f, 1.0f));		// Handles rotation
		transform = glm::scale(transform, glm::vec3(target.QScale(), target.QScale(), target.QScale()));

		// Set up our view matrix
		// We want to use orthographic projection, so set that up, and move the view matrix back 3 units so the sprites are actually in view
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		projection = glm::ortho(-4.0f * fCamZoom, +4.0f * fCamZoom, -3.f * fCamZoom, +3.f * fCamZoom, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "camMatrix"), 1, GL_FALSE, glm::value_ptr(projection * view));

		// Also need to handle transformation
		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		if (target.QScrolls())
		{
			glUniform1f(glGetUniformLocation(shaderProgram, "fTime"), fTime);
		}
		else
		{
			glUniform1f(glGetUniformLocation(shaderProgram, "fTime"), 1.0f);
		}

		// --------------------------

		// Finally, render
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	// QScrollSpeedMultiplier
	fTime += 0.001f * fScrollSpeed * TRWaveManager::QInstance()->QScrollSpeedMultiplier();
	RenderTargetStack.clear();
	RenderTargetStack.push_back(TRRenderTarget(0, 0, 0, 0, TextureLoader::QInstance().RequestTexture("Assets/Textures/T_BG_Stars.png"), 15, true));
	RenderTargetStack.push_back(TRRenderTarget(0, 0, 0, 0, TextureLoader::QInstance().RequestTexture("Assets/Textures/T_BG_Stars.png"), 20, true));

#ifdef _DEBUG
	// IMGUI -------------------------
	// Draw everything in our imGUI buffer
	for(std::shared_ptr<imguiWindow> w : imguiWindowStack)
	{
		ImGui::SetNextWindowPos(w.get()->QPos());
		ImGui::SetNextWindowSize(w.get()->QSize());

		w->Draw();
	}
	// ---------------------------------

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGui::EndFrame();
#endif

	// End of frame - swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

/// <summary>
/// Sets the camera shake to be on and off.
/// </summary>
/// <param name="abEnabled">Whether or not the camera shake is enabled.</param>
void TRRenderer::SetCameraShake(bool abEnabled)
{
	bCamShakeEnabled = abEnabled;
}
