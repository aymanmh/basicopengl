#pragma once

// Copyright(c) 2017 Packt
// MIT License (Check Licenses/Licenses.mit for full text)

#include "Scene.h"
#include "glslprogram.h"
#include "Torus.h"
#include "GLAD/glad.h"
#include "Camera.h"


class PhongScene : public Scene
{
private:
	GLSLProgram prog;

	Torus torus_m;
	float angle_m;
	OrbitCamera orbitCamera_m;
	float yaw_m = 0.0f;
	float pitch_m = 0.0f;
	float radius_m = 4.0f;
	float lastMousePosX_m = 0.0f;
	float lastMousePosY_m = 0.0f;
	const float MOUSE_SENSITIVITY = 0.5f;
	glm::vec4 worldLight_m;

	void setMatrices();
	void compileAndLinkShader();

public:
	PhongScene();

	void initScene() override;
	void update(float t) override;
	void render() override;
	void resize(int, int) override;
	void mouseMove(MouseEvent event) override;
};