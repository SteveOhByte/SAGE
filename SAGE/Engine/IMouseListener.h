#pragma once

class IMouseListener
{
public:
	virtual void OnButtonDown(int button) = 0;
	virtual void OnButtonUp(int button) = 0;
	virtual void OnButtonClick(int button) = 0;
};
