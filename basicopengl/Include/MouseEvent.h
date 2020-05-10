#pragma once

class MouseEvent
{
public:
	MouseEvent(float x, float y, bool isRight, bool isLeft);
	~MouseEvent();
	float getXPos() const;
	float getYPos() const;
	bool isLeftButton() const;
	bool isRightButton() const;
private:
	float xPos_m, yPos_m;
	bool isRight_m;
	bool isLeft_m;
};