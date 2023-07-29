#pragma once

#include "Bar_Script.h"

class HP_Script : public Bar_Script
{
public:
	HP_Script();
	virtual ~HP_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

};

