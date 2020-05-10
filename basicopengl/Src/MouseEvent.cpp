#include "MouseEvent.h"

MouseEvent::MouseEvent(float x, float y, bool isRight, bool isLeft) :
	xPos_m(x), yPos_m(y), isRight_m(isRight), isLeft_m(isLeft)
{
}

MouseEvent::~MouseEvent()
{
}

float MouseEvent::getXPos() const
{
	return xPos_m;
}

float MouseEvent::getYPos() const
{
	return yPos_m;
}

bool MouseEvent::isLeftButton() const
{
	return isLeft_m;
}

bool MouseEvent::isRightButton() const
{
	return isRight_m;
}
