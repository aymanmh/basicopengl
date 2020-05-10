#include "BasicScene.h"

#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using std::string;
#include <vector>
#include <iterator>

#include "glutils.h"

BasicScene::BasicScene() { }

void BasicScene::initScene()
{

	compileShaderProgram();

	float positionData[] = {
			-0.8f, -0.8f, 0.0f,
			0.8f, -0.8f, 0.0f,
			0.0f,  0.8f, 0.0f };
	float colorData[] = {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f };


	GLuint vboHandles[2];
	glGenBuffers(2, vboHandles);
	GLuint positionBufferHandle = vboHandles[0];
	GLuint colorBufferHandle = vboHandles[1];

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), positionData, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), colorData, GL_STATIC_DRAW);

	// Create and set-up the vertex array object
	glGenVertexArrays(1, &vaoHandle_m);
	glBindVertexArray(vaoHandle_m);

	glEnableVertexAttribArray(0);  // Vertex position
	glEnableVertexAttribArray(1);  // Vertex color

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glBindVertexArray(0);
}

void BasicScene::compileShaderProgram()
{
	try
	{
		program_m.compileShader("Assets/Shaders/basic.vert");
		program_m.compileShader("Assets/Shaders/basic.frag");
		program_m.link();
		program_m.use();
	}
	catch (GLSLProgramException& e)
	{
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}
void BasicScene::update(float t)
{

}

void BasicScene::render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(vaoHandle_m);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}

void BasicScene::resize(int w, int h)
{
	width_m = w;
	height_m = h;
	glViewport(0, 0, w, h);
}