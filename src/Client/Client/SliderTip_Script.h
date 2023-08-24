#pragma once

#include "Button_Script.h"

class SliderTip_Script final : public Button_Script
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
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

private:
	void ChangeVolume();

private:
	SLIDER_TYPE m_sliderType;
};

