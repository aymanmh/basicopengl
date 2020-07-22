#include "GlowScene.h"

#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <ctime>
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

float lmap(float val, float inMin, float inMax, float outMin, float outMax)
{
	return outMin + ((outMax - outMin) * (val - inMin)) / (inMax - inMin);
}

class MyPoint
{
public:
	MyPoint(vec3 _pos, vec3 _color, float _size = 130.0f) : pos(_pos), color(_color), brightness(1.0f),size(_size) {};
	vec3 pos;
	vec3 color;
	float brightness;
	float size;
	static size_t const dataSize = sizeof(vec3) * 2 + 2 * sizeof(float);
	
	void glow() 
	{
		brightness = lmap( sinf(start) , -1.0f,1.0f,1.0f,2.8f);

		start += brightnessSpeed;		
	};
private:
	float brightnessSpeed = 0.02f;
	float start = -3.1415926/2.0;
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

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, nullptr);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));


		glBindVertexArray(0);
	}

	~Quad() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}

	GLuint vao, vbo;
};


GlowScene::GlowScene()
{ }

void GlowScene::initScene()
{
	srand((unsigned)time(0));
	glEnable(GL_PROGRAM_POINT_SIZE);
	compileShaderProgram();

	myPoints_m.emplace_back(vec3(100.0f, 100.15f, 0.0f), vec3(0.0f, 0.6f, 0.9f), float(rand() % 100) +100.0f);
	myPoints_m.emplace_back(vec3(100.0f, 600.15f, 0.0f), vec3(0.3f, 0.8f, 0.1f), float(rand() % 100) + 100.0f);
	myPoints_m.emplace_back(vec3(600.0f, 300.15f, 0.0f), vec3(0.9f, 0.6f, 0.3f), float(rand() % 100) + 100.0f);

	/*
	cout << MyPoint::dataSize << endl;;
	cout << sizeof(MyPoint) << endl;
	cout << offsetof(MyPoint, pos)<< endl;
	cout << offsetof(MyPoint, color)<< endl;
	cout << offsetof(MyPoint, brightness)<< endl;
	cout << offsetof(MyPoint, brightnessSpeed)<< endl;
	*/
	glGenBuffers(1, &spirtesVbo_m);
	glBindBuffer(GL_ARRAY_BUFFER, spirtesVbo_m);
	glBufferData(GL_ARRAY_BUFFER, myPoints_m.size() * MyPoint::dataSize , NULL, GL_DYNAMIC_DRAW);
	for (int i = 0; i < myPoints_m.size(); i++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, i* MyPoint::dataSize, MyPoint::dataSize, &myPoints_m[i]);
	}
	glGenVertexArrays(1, &vaoHandle_m);
	glBindVertexArray(vaoHandle_m);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, MyPoint::dataSize, NULL);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, MyPoint::dataSize, (GLvoid*)(offsetof(MyPoint, color)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, MyPoint::dataSize, (GLvoid*)(offsetof(MyPoint, brightness)));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, MyPoint::dataSize, (GLvoid*)(offsetof(MyPoint, size)));

	glBindVertexArray(0);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4  view = glm::mat4(1.0f);
	glm::mat4  projection;;
	projection = glm::ortho(0.0f, (float)width_m, 0.0f, (float)height_m, -1.0f, 1.0f);
	mvp_m = projection * view;
	program_m.setUniform("mvp", mvp_m);

	flair_m.loadTexture("Assets/Images/flair2.png");
	space_m.loadTexture("Assets/Images/space.png",false);

	quad_m = std::make_unique<Quad>();

	setupFBO();
	//flair_m.bind();
	//space_m.bind();

	computeGaussianWeights();

	program_m.use();
}

void GlowScene::compileShaderProgram()
{
	try
	{
		program_m.compileShader("Assets/Shaders/GlowSprite.vert");
		program_m.compileShader("Assets/Shaders/GlowSprite.frag");
		program_m.link();
		program_m.use();
	}
	catch (GLSLProgramException& e)
	{
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void GlowScene::update(float t)
{
	for (auto &point: myPoints_m)
	{
		point.glow();
	}

	glBindBuffer(GL_ARRAY_BUFFER, spirtesVbo_m);	
	//glBufferSubData(GL_ARRAY_BUFFER, 0, myPoints_m.size() * sizeof(MyPoint), &myPoints_m[0]);
	//glBufferData(GL_ARRAY_BUFFER, myPoints_m.size() * sizeof(MyPoint), &myPoints_m[0], GL_DYNAMIC_DRAW);
	
	for (int i = 0; i < myPoints_m.size(); i++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, i * MyPoint::dataSize, MyPoint::dataSize, &myPoints_m[i]);
	}	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GlowScene::render()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	
	//drawScene();

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	//bool show_demo_window = true;
	//ImGui::ShowDemoWindow(&show_demo_window);
	drawIMGUI();


	
	renderToTexture();
	hBlur();
	vBlur();
	
	if (ShowHDR_m || ShowBrightness_m || ShowBlur_m)
		drawFBO();
	else
		finalDraw();
	
	
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GlowScene::renderToTexture()
{
	glViewport(0, 0, width_m, height_m);

	glBindFramebuffer(GL_FRAMEBUFFER, hdrFbo_m);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	program_m.setUniform("LumThresh", luminanceThresh_m);
	program_m.setUniform("exposure", exposure_m);

	program_m.setUniform("Pass", 1);
	program_m.setUniform("mvp", mvp_m);
	flair_m.bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vaoHandle_m);
	//glPointSize(0);
	glDrawArrays(GL_POINTS, 0, myPoints_m.size());
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void GlowScene::vBlur()
{
	glViewport(0, 0, width_m, height_m);

	glBindFramebuffer(GL_FRAMEBUFFER, blurFBO_m);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	program_m.setUniform("Pass", 2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, brightnessTexture_m);

	program_m.setUniform("mvp", mat4(1));
	program_m.setUniform("Pass", 2);
	glBindVertexArray(quad_m->vao);
	
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void GlowScene::hBlur()
{
	glViewport(0, 0, width_m, height_m);

	glBindFramebuffer(GL_FRAMEBUFFER, blurFBO_m);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTexture2_m, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blurTexture1_m);

	program_m.setUniform("mvp", mat4(1));
	program_m.setUniform("Pass", 3);
	
	glBindVertexArray(quad_m->vao);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void GlowScene::finalDraw()
{
	glViewport(0, 0, width_m, height_m);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.2f, 0.2f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	program_m.setUniform("mvp", mat4(1));
	//program_m.setUniform("Pass", 5);
	//space_m.bind();
	
	//glBindVertexArray(quad_m->vao);
	//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blurTexture2_m);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, hdrTexture_m);

	program_m.setUniform("Pass", 4);
	program_m.setUniform("bloom", bloom_m);
	program_m.setUniform("exposure", exposure_m);
	program_m.setUniform("gamma", gamma_m);

	glBindVertexArray(quad_m->vao);
	glBindVertexArray(quad_m->vao);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void GlowScene::drawScene(int pass)
{
	program_m.setUniform("Pass", pass);
	program_m.setUniform("mvp", mvp_m);
	flair_m.bind();
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glBindVertexArray(vaoHandle_m);
	//glPointSize(130.f);
	glDrawArrays(GL_POINTS, 0, myPoints_m.size());
	glBindVertexArray(0);
	//glBindTexture(GL_TEXTURE_2D, 0);

}

void GlowScene::resize(int w, int h)
{
	width_m = w;
	height_m = h;
	glViewport(0, 0, w, h);
}

void GlowScene::mouseDown(MouseEvent event)
{

	float invertY = glm::abs(height_m - event.getYPos());
	//myPoints_m.emplace_back(MyPoint(vec3(event.getXPos(), invertY,0.0f),vec3(0.20f,0.20f,0.05f)));
	vec3 color = vec3(float(rand() % 100) / 100.0f, float(rand() % 100) / 100.0f, float(rand() % 100) / 100.0f);
	myPoints_m.emplace_back(MyPoint(vec3(event.getXPos(), invertY,0.0f), color, float(rand() % 100) + 100.0f));

	glBindBuffer(GL_ARRAY_BUFFER, spirtesVbo_m);
	//glBufferData(GL_ARRAY_BUFFER, myPoints_m.size() * sizeof(MyPoint), &myPoints_m[0], GL_DYNAMIC_DRAW);	
	
	glBufferData(GL_ARRAY_BUFFER, myPoints_m.size() * MyPoint::dataSize, NULL, GL_DYNAMIC_DRAW);
	for (int i = 0; i < myPoints_m.size(); i++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, i * MyPoint::dataSize, MyPoint::dataSize, &myPoints_m[i]);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void GlowScene::drawFBO()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.9f, 0.1f, 1.0f);

	program_m.setUniform("mvp", mat4(1));
	program_m.setUniform("Pass", 5);

	space_m.bind();
	glBindVertexArray(quad_m->vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	
	glActiveTexture(GL_TEXTURE0);
	if(ShowBrightness_m)
		glBindTexture(GL_TEXTURE_2D, brightnessTexture_m);
	else if(ShowHDR_m)
		glBindTexture(GL_TEXTURE_2D, hdrTexture_m);
	else
		glBindTexture(GL_TEXTURE_2D, blurTexture2_m);

	glGenerateMipmap(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, width_m, height_m);  // Viewport for the texture
	
	glBindVertexArray(quad_m->vao);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//basicProgram_m.use();
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	//glBindTexture(GL_TEXTURE_2D, 0);
	//GLUtils::checkForOpenGLError(__FILE__, __LINE__);

	glBindVertexArray(0);

}

void GlowScene::drawIMGUI()
{
	ImGui::Begin("Demo window");
	ImGui::Checkbox("Show HDR", &ShowHDR_m);
	ImGui::Checkbox("Show Brightness", &ShowBrightness_m);
	ImGui::Checkbox("Show Blur", &ShowBlur_m);
	ImGui::Checkbox("Bloom", &bloom_m);
	ImGui::SliderFloat("Exposure", &exposure_m, 0.1f, 10.0f);
	ImGui::SliderFloat("Luminance", &luminanceThresh_m, 0.0f, 2.0f);
	ImGui::SliderFloat("Gamma", &gamma_m, 0.1f, 10.0f);
	if (ImGui::SliderFloat("Sigma", &sigma_m, 0.1f, 20.0f))
		computeGaussianWeights();

	ImGui::Text("FPS:%d",static_cast<int>(fps_m));
	ImGui::End();
}

void GlowScene::computeGaussianWeights()
{
	float sum, sigma2 = sigma_m;

	// Compute and sum the weights
	weights_m[0] = gaussian(0, sigma2);
	sum = weights_m[0];
	for (int i = 1; i < 5; i++) {
		weights_m[i] = gaussian(float(i), sigma2);
		sum += 2 * weights_m[i];
	}

	// Normalize the weights and set the uniform
	for (int i = 0; i < 5; i++) {
		std::stringstream uniName;
		uniName << "Weight[" << i << "]";
		float val = weights_m[i] / sum;
		program_m.setUniform(uniName.str().c_str(), val);
	}
}

float GlowScene::gaussian(float x, float sigma2)
{
	double coeff = 1.0 / (glm::two_pi<double>() * sigma2);
	double expon = -(x * x) / (2.0 * sigma2);
	return (float)(coeff * exp(expon));
}

void GlowScene::setupFBO() {
	// Generate and bind the framebuffer
	glGenFramebuffers(1, &hdrFbo_m);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFbo_m);

	for (int i = 0; i < 2; i++)
	{		// Create thes texture object
		glGenTextures(1, i==0 ? &hdrTexture_m : &brightnessTexture_m);
		glActiveTexture(GL_TEXTURE0);  // Use texture unit 0
		glBindTexture(GL_TEXTURE_2D, i == 0 ? hdrTexture_m : brightnessTexture_m);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width_m, height_m);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		GLUtils::checkForOpenGLError(__FILE__, __LINE__);
		// Bind the texture to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, i == 0 ? hdrTexture_m : brightnessTexture_m, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
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
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1 };
	//GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0};
	glDrawBuffers(2, drawBuffers);

	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result == GL_FRAMEBUFFER_COMPLETE) {
		cout << "Framebuffer is complete" << endl;
	}
	else {
		cout << "Framebuffer error: " << result << endl;
	}

	// Unbind the framebuffer, and revert to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glBindTexture(GL_TEXTURE_2D, 0);
	// Generate and bind the framebuffer
	glGenFramebuffers(1, &blurFBO_m);
	glBindFramebuffer(GL_FRAMEBUFFER, blurFBO_m);

	glGenTextures(1, &blurTexture1_m);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, blurTexture1_m);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width_m, height_m);

	glActiveTexture(GL_TEXTURE2);
	glGenTextures(1, &blurTexture2_m);
	glBindTexture(GL_TEXTURE_2D, blurTexture2_m);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width_m, height_m);

	// Bind tex1 to the FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTexture1_m, 0);

	glDrawBuffers(1, drawBuffers);

	// Unbind the framebuffer, and revert to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}