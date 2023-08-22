#pragma once

#include "Button_Script.h"

class SliderTip_Script : public Button_Script
{
public:
	SliderTip_Script();
	virtual ~SliderTip_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;


};

