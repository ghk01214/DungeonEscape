#pragma once

#include "Button_Script.h"

class SliderTip_Script : public Button_Script
{
public:
	enum class SLIDER_TYPE
	{
		NONE = 0,

		BGM,
		SE,

		MAX
	};
public:
	SliderTip_Script(SLIDER_TYPE type);
	virtual ~SliderTip_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

private:
	void ChangeVolume();

private:
	SLIDER_TYPE m_sliderType;
};

