#pragma once


class ShaderManagement
{
public:

	int GenerateShaderProgram();

private:
	const char* sVertexShaderSource = R"HERE(
	#version 330 core

	layout (location = 0) in vec3 aPos;
	layout (location = 1) in vec3 aColor;
	layout (location = 2) in vec2 aTexCoord;

	out vec3 ourColor;
	out vec2 TexCoord;

	uniform mat4 camMatrix;
	uniform mat4 transform;

	void main()
	{
		gl_Position = camMatrix * transform * vec4(aPos, 1);
		ourColor = aColor;
		TexCoord = vec2(aTexCoord.x, aTexCoord.y);
	}
)HERE";

	const char* sFragmentShaderSource = R"HERE(
	#version 330 core

	out vec4 FragColor;

	in vec3 ourColor;
	in vec2 TexCoord;

	uniform sampler2D texture01;
	uniform float fTime;

	void main()
	{
		FragColor = texture(texture01, vec2(TexCoord.x + fTime, TexCoord.y));
	}
)HERE";

};

