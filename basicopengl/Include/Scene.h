#pragma once

// Copyright(c) 2017 Packt
// MIT License (Check Licenses/Licenses.mit for full text)

#include <glm.hpp>
#include "MouseEvent.h"

class Scene
{
protected:
	glm::mat4 model, view, projection;

public:
	int width_m;
	int height_m;

	Scene() : animate_m(true), width_m(800), height_m(600) { }
	virtual ~Scene() {}

	void setDimensions(int w, int h) {
		width_m = w;
		height_m = h;
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

	virtual void mouseDrag(MouseEvent event) {};

	void animate(bool value) { animate_m = value; }
	bool animating() { return animate_m; }

protected:
	bool animate_m;
};