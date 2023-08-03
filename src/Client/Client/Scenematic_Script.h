#pragma once

#include "MonoBehaviour.h"

class Scenematic_Script : public MonoBehaviour
{
public:
	Scenematic_Script();
	~Scenematic_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
};