#include <iostream>
#include "SceneRunner.h"
#include "BasicScene.h"
#include "SpriteScene.h"
#include "GlowScene.h"
#include "PhongScene.h"
#include "CloudScene.h"

int main()
{
	std::string title = "Test Window";
	SceneRunner myRunner(title,1280,720);
	std::unique_ptr<Scene> scene;

	//scene = std::unique_ptr<Scene>(new BasicScene());
	//scene = std::unique_ptr<Scene>(new SpriteScene());
	//scene = std::unique_ptr<Scene>(new GlowScene());
	//scene = std::unique_ptr<Scene>(new PhongScene());
	scene = std::unique_ptr<Scene>(new CloudScene());
	
	myRunner.setScene(std::move(scene));
	return myRunner.run();
}