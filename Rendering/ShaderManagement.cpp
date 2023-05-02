#include "ShaderManagement.h"

#include <iostream>	// Temp iostream include, replace with our proper debugger later

#include <glad/glad.h>
#include <GLFW/glfw3.h>

/// <summary>
/// Generates the Shader Program for the application. This compiles our vertex and fragment shaders, and links them, returning the final bound shader program.
/// This also handles removing the raw shaders from memory when done with.
/// Please note: this engine does not currently support custom shader types, and this will not change. This compiles a pre-written Vert and Frag shader found in ShaderManagement.h
/// </summary>
/// <returns>int constaining the final Shader Program, used for setting textures and such for objects..</returns>
int ShaderManagement::GenerateShaderProgram()
{
	int iSuccess;

	// Create our functor for compiling our shaders
	auto compileShaderFunctor = [](auto sShaderSource, int iShaderType, int* iShaderOut, int* iSuccessOut)
	{
		char cInfoLog[512];
		*iShaderOut = glCreateShader(iShaderType);
		glShaderSource(*iShaderOut, 1, &sShaderSource, NULL);
		glCompileShader(*iShaderOut);
		// Check for any shader compile errors
		glGetShaderiv(*iShaderOut, GL_COMPILE_STATUS, iSuccessOut);
		if (!iSuccessOut)
		{
			glGetShaderInfoLog(*iShaderOut, 512, NULL, cInfoLog);
			std::cout << "ERROR| Shader Compilation error.\n" << cInfoLog << std::endl;
		}
	};

	int vertextShader = 0;
	int fragmentShader = 0;
	compileShaderFunctor(sVertexShaderSource, GL_VERTEX_SHADER, &vertextShader, &iSuccess);
	compileShaderFunctor(sFragmentShaderSource, GL_FRAGMENT_SHADER, &fragmentShader, &iSuccess);

	// Link shaders into a single shader program
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertextShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Check for linking errors
	char cInfoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &iSuccess);
	if (!iSuccess)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, cInfoLog);
		std::cout << "ERROR| Shader linking error.\n" << cInfoLog << std::endl;
	}
	// Clear raw shaders from memory
	glDeleteShader(vertextShader);
	glDeleteShader(fragmentShader);
	// --------------------------

	glUseProgram(shaderProgram);
	return shaderProgram;
}
