#include "PhongScene.h"

#include <iostream>
using std::cerr;
using std::endl;

#include <gtc/matrix_transform.hpp>
#include "..\Include\PhongScene.h"
using glm::vec3;
using glm::mat4;

PhongScene::PhongScene() :
    angle(0.0f),
    torus(0.7f, 0.3f, 50, 50)
{ }

void PhongScene::initScene()
{
    compileAndLinkShader();

    glEnable(GL_DEPTH_TEST);

    view = glm::lookAt(vec3(0.0f, 0.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f);
    glm::vec4 worldLight = glm::vec4(5.0f, 5.0f, 2.0f, 1.0f);

    prog.setUniform("Material.Kd", 0.9f, 0.5f, 0.3f);
    prog.setUniform("Light.Ld", 1.0f, 1.0f, 1.0f);
    prog.setUniform("Light.Position", view * worldLight);
    prog.setUniform("Material.Ka", 0.9f, 0.5f, 0.3f);
    prog.setUniform("Light.La", 0.4f, 0.4f, 0.4f);
    prog.setUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
    prog.setUniform("Light.Ls", 1.0f, 1.0f, 1.0f);
    prog.setUniform("Material.Shininess", 100.0f);
}

void PhongScene::update(float t) { }

void PhongScene::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(angle), vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-35.0f), vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(35.0f), vec3(0.0f, 1.0f, 0.0f));

    setMatrices();
    torus.render();
}

void PhongScene::setMatrices()
{
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix",
        glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
}

void PhongScene::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width_m = w;
    height_m = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void PhongScene::compileAndLinkShader()
{
    try {
        prog.compileShader("Assets/Shaders/phong.vert");
        prog.compileShader("Assets/Shaders/phong.frag");
        //prog.compileShader("shader/function.vert.glsl");
        //prog.compileShader("shader/function.frag.glsl");
        prog.link();
        prog.use();
    }
    catch (GLSLProgramException& e) {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}
