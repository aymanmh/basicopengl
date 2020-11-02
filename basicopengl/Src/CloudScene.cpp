#include "CloudScene.h"
#include "NoiseTexture.h"

#include <iostream>
using std::cerr;
using std::endl;

#include <gtc/matrix_transform.hpp>
using glm::vec3;
using glm::mat4;


CloudScene::CloudScene() { }

void CloudScene::initScene()
{
    compileAndLinkShader();

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    projection_m = mat4(1.0f);
    // Array for quad
    GLfloat verts[] = {
        -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
    };
    GLfloat tc[] = {
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };

    // Set up the buffers
    unsigned int handle[2];
    glGenBuffers(2, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

    // Set up the vertex array object
    glGenVertexArrays(1, &quad_m);
    glBindVertexArray(quad_m);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
    glEnableVertexAttribArray(2);  // Texture coordinates

    glBindVertexArray(0);

    program_m.setUniform("NoiseTex", 0);

    GLuint noiseTex = NoiseTexture::generatePeriodic2DTex(7.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, noiseTex);
}

void CloudScene::update(float t) { }

void CloudScene::render()
{
    view_m = mat4(1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawScene();
    glFinish();
}

void CloudScene::drawScene()
{
    model_m = mat4(1.0f);
    setMatrices();

    glBindVertexArray(quad_m);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void CloudScene::setMatrices()
{
    mat4 mv = view_m * model_m;
    program_m.setUniform("MVP", projection_m * mv);
}

void CloudScene::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width_m = w;
    height_m = h;
}

void CloudScene::compileAndLinkShader()
{
    try {
        program_m.compileShader("Assets/Shaders/cloud.vert", GLSLShader::VERTEX);
        program_m.compileShader("Assets/Shaders/cloud.frag", GLSLShader::FRAGMENT);
        program_m.link();
        program_m.use();
    }
    catch (GLSLProgramException& e) {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}