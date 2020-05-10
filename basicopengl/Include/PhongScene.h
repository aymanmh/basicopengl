#pragma once
#pragma once

#include "Scene.h"
#include "glslprogram.h"
#include "Torus.h"
#include "GLAD/glad.h"

class PhongScene : public Scene
{
private:
	GLSLProgram prog;

	Torus torus;
	float angle;

	void setMatrices();
	void compileAndLinkShader();

public:
	PhongScene();

	void initScene();
	void update(float t);
	void render();
	void resize(int, int);
};