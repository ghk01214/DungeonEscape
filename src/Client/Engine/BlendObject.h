#pragma once
#include "Component.h"

class BlendObject : public Component
{
public:
	BlendObject();
	virtual ~BlendObject();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FinalUpdate() override;
};

