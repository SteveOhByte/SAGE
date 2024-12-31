#pragma once
#include "Component.h"
#include "../Utilities/IColliderListener.h"
#include "../Utilities/IKeyboardListener.h"
#include "../Utilities/IMouseListener.h"

class Behaviour : public Component, public IKeyboardListener, public IMouseListener, public IColliderListener
{
public:
	Behaviour() : Component(ComponentType::BEHAVIOUR) {}

	virtual void Start() = 0;
	virtual void Tick(float deltaTime, float2 mousePos) = 0;

	void OnKeyDown(UINT keyCode) override;
	void OnKeyUp(UINT keyCode) override;
	void OnKeyPress(UINT keyCode) override;

	void OnButtonDown(int button) override;
	void OnButtonUp(int button) override;
	void OnButtonClick(int button) override;

	void OnCollisionEnter(GameObject* other) override;
	void OnCollisionExit(GameObject* other) override;
	void OnTriggerEnter(GameObject* other) override;
	void OnTriggerExit(GameObject* other) override;
};
