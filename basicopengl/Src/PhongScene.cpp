#include "PhongScene.h"

#include <iostream>
using std::cerr;
using std::endl;

#include <gtc/matrix_transform.hpp>
#include "..\Include\PhongScene.h"
using glm::vec3;
using glm::mat4;

PhongScene::PhongScene() :
    angle_m(0.0f),
    torus_m(0.7f, 0.3f, 50, 50)
{ }

void PhongScene::initScene()
{
    compileAndLinkShader();

    glEnable(GL_DEPTH_TEST);

    glm::vec3 torusPos = glm::vec3(0.f, 0.f, 0.0f);
    
    orbitCamera_m.setLookAt(torusPos);
    orbitCamera_m.setRadius(radius_m);
    orbitCamera_m.roate(yaw_m, pitch_m);

    view_m = orbitCamera_m.getViewMatrix();
    projection_m = mat4(1.0f);
    worldLight_m = glm::vec4(5.0f, 5.0f, 2.0f, 1.0f);

    prog.setUniform("Material.Kd", 0.9f, 0.5f, 0.3f);
    prog.setUniform("Light.Ld", 1.0f, 1.0f, 1.0f);
    prog.setUniform("Light.Position", view_m * worldLight_m);
    prog.setUniform("Material.Ka", 0.9f, 0.5f, 0.3f);
    prog.setUniform("Light.La", 0.4f, 0.4f, 0.4f);
    prog.setUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
    prog.setUniform("Light.Ls", 1.0f, 1.0f, 1.0f);
    prog.setUniform("Material.Shininess", 100.0f);

    model_m = mat4(1.0f);
    model_m = glm::rotate(model_m, glm::radians(angle_m), vec3(0.0f, 1.0f, 0.0f));
    model_m = glm::rotate(model_m, glm::radians(-35.0f), vec3(1.0f, 0.0f, 0.0f));
    model_m = glm::rotate(model_m, glm::radians(35.0f), vec3(0.0f, 1.0f, 0.0f));
}

void PhongScene::update(float t) 
{
    prog.setUniform("Light.Position", view_m * worldLight_m); // to keep the light fixed when the camera rotates
    setMatrices();
}

void PhongScene::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    torus_m.render();
}

void PhongScene::setMatrices()
{
    view_m = orbitCamera_m.getViewMatrix();
    mat4 mv = view_m * model_m;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix",
        glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection_m * mv);
}

void PhongScene::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width_m = w;
    height_m = h;
    projection_m = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
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

void PhongScene::mouseMove(MouseEvent event)
{
    auto posX = event.getXPos();
    auto posY = event.getYPos();

    if (event.isLeftButton()) // rotate
    {
        yaw_m -= (posX - lastMousePosX_m) * MOUSE_SENSITIVITY;
        pitch_m += (posY - lastMousePosY_m) * MOUSE_SENSITIVITY;
        orbitCamera_m.roate(yaw_m, pitch_m);
    }

    if (event.isRightButton()) // zoom in/out
    {
        float dx = 0.01f * (posX - lastMousePosX_m);
        float dy = 0.01f * (posY - lastMousePosY_m);
        radius_m += dx - dy;
        orbitCamera_m.setRadius(radius_m);
    }

    lastMousePosX_m = posX;
    lastMousePosY_m = posY;
}
