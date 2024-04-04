#pragma once

class IColliderListener
{
public:
	virtual void OnCollisionEnter(GameObject* other) = 0;
	virtual void OnCollisionExit(GameObject* other) = 0;
	
	virtual void OnTriggerEnter(GameObject* other) = 0;
	virtual void OnTriggerExit(GameObject* other) = 0;
};
