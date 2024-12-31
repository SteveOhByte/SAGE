#pragma once
#include "../Components/AnimatedSprite.h"

class IKeyboardListener
{
public:
	virtual void OnKeyDown(UINT keyCode) = 0;
	virtual void OnKeyUp(UINT keyCode) = 0;
	virtual void OnKeyPress(UINT keyCode) = 0;
};
