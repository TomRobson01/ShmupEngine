#include <iostream>
#include <vector>

#include "DebugWindows/TestImGUIWindow.h"

#include "TRRenderer.h"
#include "Rendering/ShaderManagement.h"

#define SCR_WIDTH 800
#define SCR_HEIGHT 600

std::vector<TRRenderTarget> RenderTargetStack;
std::atomic<bool> RenderThreadLock;

// OpenGL resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	std::cout << "New Resolution:\n" << width << ", " << height << std::endl;
	glViewport(0, 0, width, height);
}

TRRenderer::TRRenderer()
{
	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create our glfw window
	window = glfwCreateWindow(START_WIDTH, START_HEIGHT, "LearnOpenGL", NULL, NULL);
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
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
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

#ifdef _DEBUG
	std::shared_ptr<imguiWindow> someWindow =  std::make_shared<TestImGUIWindow>();
	someWindow->CreateWindow("Some window", ImVec2(0,0), ImVec2(512, 512), 0);
	AddImguiWindowToRender(someWindow);
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

void TRRenderer::AddRenderTarget(float aiX, float aiY, float aiZ, float afRotation, unsigned int aiTexture)
{
	RenderTargetStack.push_back(TRRenderTarget(aiX, aiY, aiZ, afRotation, aiTexture));
}

void TRRenderer::RenderStack()
{
	// Render to buffer
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

#ifdef _DEBUG
	// Prep ImGUI
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(100, 100));
#endif

	// All this needs to happen for each render target
	for (TRRenderTarget target : RenderTargetStack)
	{
		// Bind the texture we want to use
		glBindTexture(GL_TEXTURE_2D, target.QTexture());

		// Transformation code 
		// -----------------------
		// Create transformations
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), target.QPosition());		// Creates initial transformation
		transform = glm::rotate(transform, target.QRotation(), glm::vec3(0.0f, 0.0f, 1.0f));		// Handles rotation

		// Set up our view matrix
		// We want to use orthographic projection, so set that up, and move the view matrix back 3 units so the sprites are actually in view
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "camMatrix"), 1, GL_FALSE, glm::value_ptr(projection * view));

		// Also need to handle transformation
		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		// --------------------------

		// Finally, render
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	RenderTargetStack.clear();

#ifdef _DEBUG
	// IMGUI -------------------------
	// Draw everything in our imGUI buffer
	for(std::shared_ptr<imguiWindow> w : imguiWindowStack)
	{
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
