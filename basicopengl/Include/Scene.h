#pragma once

// Copyright(c) 2017 Packt
// MIT License (Check Licenses/Licenses.mit for full text)

#include <glm.hpp>
#include "MouseEvent.h"

class Scene
{
public:

	Scene() : animate_m(true), width_m(800), height_m(600) { }
	virtual ~Scene() {}

	void setDimensions(int w, int h) 
	{
		width_m = w;
		height_m = h;
	}

	void setFPS(double fps)
	{
		fps_m = fps;
	}

	/**
	  Load textures, initialize shaders, etc.
	  */
	virtual void initScene() = 0;

	/**
	  This is called prior to every frame.  Use this
	  to update your animation.
	  */
	virtual void update(float t) = 0;

	/**
	  Draw your scene.
	  */
	virtual void render() = 0;

	/**
	  Called when screen is resized
	  */
	virtual void resize(int, int) = 0;

	virtual void mouseDown(MouseEvent event) {};

	virtual void mouseMove(MouseEvent event) {};

	virtual void mouseScroll(double deltaX, double deltaY) {};

	void animate(bool value) { animate_m = value; }
	bool animating() { return animate_m; }

protected:
	glm::mat4 model_m, view_m, projection_m;
	bool animate_m;
	int width_m;
	int height_m;
	double fps_m;
};