#include "SpriteScene.h"

#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <glm.hpp>
#include <ext\matrix_transform.hpp>
#include <ext\matrix_clip_space.hpp>
#include "glutils.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using std::string;
using namespace std;
using namespace glm;

class MyPoint
{
public:
	MyPoint(vec3 _pos, vec3 _color) : pos(_pos), color(_color) {};
	vec3 pos;
	vec3 color;
};

class Quad
{
public:
	Quad() {
		GLfloat vertices[] = { // format = x, y, z, u, v
			-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f
		};

		glGenVertexArrays(1, &vao); // vao saves state of array buffer, element array, etc
		glGenBuffers(1, &vbo); // vbo stores vertex data

		GLint curr_vao; // original state
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &curr_vao);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, nullptr);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(curr_vao);
	}

	~Quad() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}

	GLuint vao, vbo;
};

SpriteScene::SpriteScene() { }

void SpriteScene::initScene()
{
	compileShaderProgram();

	myPoints_m.emplace_back(vec3(100.0f, 100.15f, 0.0f), vec3(0.0f, 0.6f, 0.3f));
	myPoints_m.emplace_back(vec3(100.0f, 600.15f, 0.0f), vec3(0.3f, 0.6f, 0.3f));
	myPoints_m.emplace_back(vec3(600.0f, 300.15f, 0.0f), vec3(0.3f, 0.6f, 0.3f));
	//myPoints.emplace_back(vec3(0.0f, 0.9f, 0.0f), vec3(0.0f, 0.6f, 0.3f));
	//myPoints.emplace_back(vec3(0.15f, -0.15f, 0.0f), vec3(0.98f, 0.55f, 0.0f));
	//myPoints.emplace_back(vec3(-0.15f, -0.15f, 0.0f), vec3(0.0f, 0.31f, 0.61f));

	//GLuint spirtesVbo;

	glGenBuffers(1, &spirtesVbo);
	glBindBuffer(GL_ARRAY_BUFFER, spirtesVbo);
	glBufferData(GL_ARRAY_BUFFER, myPoints_m.size() * sizeof(MyPoint), &myPoints_m[0], GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &vaoHandle_m);
	glBindVertexArray(vaoHandle_m);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), NULL);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), (GLvoid*)(offsetof(MyPoint, color)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4  view = glm::mat4(1.0f);
	glm::mat4  mvp, projection;;
	projection = glm::ortho(0.0f, (float)width_m, 0.0f, (float)height_m, -1.0f, 1.0f);
	mvp = projection * view;
	program_m.setUniform("mvp", mvp);

	flair_m.loadTexture("Assets/Images/flair2.png");
	//space_m.loadTexture("Images/space.png",false);

	quad_m = std::make_unique<Quad>();

	setupFBO();
	//flair_m.bind();
	//space_m.bind();

}

void SpriteScene::compileShaderProgram()
{
	try
	{
		program_m.compileShader("Assets/Shaders/basicSprite.vert");
		program_m.compileShader("Assets/Shaders/basicSprite.frag");
		program_m.link();
		program_m.use();
		
		basicProgram_m.compileShader("Assets/Shaders/texture.vert");
		basicProgram_m.compileShader("Assets/Shaders/texture.frag");
		basicProgram_m.link();
	}
	catch (GLSLProgramException& e)
	{
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SpriteScene::update(float t)
{	
}

void SpriteScene::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//drawScene();

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	//bool show_demo_window = true;
	//ImGui::ShowDemoWindow(&show_demo_window);
	drawIMGUI();



	//return;

	renderToTexture();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//glViewport(0, 0, width_m, height_m);	
	if(ShowFBO_m)
		drawFBO();
	else
		drawScene();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SpriteScene::renderToTexture()
{
	glViewport(0, 0, width_m, height_m);

	glBindFramebuffer(GL_FRAMEBUFFER, hdrFbo_m);
	glEnable(GL_DEPTH_TEST);
	drawScene();
}

void SpriteScene::drawScene()
{

	program_m.use();
	flair_m.bind();

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vaoHandle_m);
	glPointSize(100.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.25f, 0.0f, 1.0f);
	glDrawArrays(GL_POINTS, 0, myPoints_m.size());
	glBindVertexArray(0);
	//glBindTexture(GL_TEXTURE_2D, 0);

}

void SpriteScene::resize(int w, int h)
{
	width_m = w;
	height_m = h;
	glViewport(0, 0, w, h);
}

void SpriteScene::mouseDown(MouseEvent event)
{

	float invertY = glm::abs(height_m - event.getYPos());
	myPoints_m.emplace_back(MyPoint(vec3(event.getXPos(), invertY,0.0f),vec3(0.20f,0.20f,0.05f)));

	glBindBuffer(GL_ARRAY_BUFFER, spirtesVbo);
	glBufferData(GL_ARRAY_BUFFER, myPoints_m.size() * sizeof(MyPoint), &myPoints_m[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SpriteScene::setupFBO() {
	// Generate and bind the framebuffer
	glGenFramebuffers(1, &hdrFbo_m);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFbo_m);

	// Create thes texture object
	glGenTextures(1, &renderTex_m);
	glActiveTexture(GL_TEXTURE0);  // Use texture unit 0
	glBindTexture(GL_TEXTURE_2D, renderTex_m);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, width_m, height_m);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GLUtils::checkForOpenGLError(__FILE__, __LINE__);
	// Bind the texture to the FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex_m, 0);

	// Create the depth buffer
	GLuint depthBuf;
	glGenRenderbuffers(1, &depthBuf);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width_m, height_m);
	GLUtils::checkForOpenGLError(__FILE__, __LINE__);
	// Bind the depth buffer to the FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, depthBuf);
	GLUtils::checkForOpenGLError(__FILE__, __LINE__);
	// Set the targets for the fragment output variables
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result == GL_FRAMEBUFFER_COMPLETE) {
		cout << "Framebuffer is complete" << endl;
	}
	else {
		cout << "Framebuffer error: " << result << endl;
	}

	// Unbind the framebuffer, and revert to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void SpriteScene::drawFBO()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.2f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, renderTex_m);
	glGenerateMipmap(GL_TEXTURE_2D);

	glViewport(0, 0, width_m, height_m);  // Viewport for the texture

	glBindVertexArray(quad_m->vao);

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	basicProgram_m.use();
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	//glBindTexture(GL_TEXTURE_2D, 0);
	//GLUtils::checkForOpenGLError(__FILE__, __LINE__);

	glBindVertexArray(0);

}

void SpriteScene::drawIMGUI()
{
	ImGui::Begin("Demo window");
	ImGui::Checkbox("Show FBO", &ShowFBO_m);
	ImGui::End();
}
