#pragma once

// Copyright(c) 2017 Packt
// MIT License (Check Licenses/Licenses.mit for full text)

#include "scene.h"
#include "GLSLProgram.h"

using namespace std;

class CloudScene : public Scene
{
private:
    GLSLProgram program_m;

    GLuint quad_m;

    glm::vec3 lightPos_m;
    float angle_m;

    void setMatrices();
    void compileAndLinkShader();
    void drawScene();

public:
    CloudScene();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};