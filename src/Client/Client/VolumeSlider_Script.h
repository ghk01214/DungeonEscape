#pragma once

#include "Bar_Script.h"

class VolumeSlider_Script : public Bar_Script
{
public:
	VolumeSlider_Script();
	virtual ~VolumeSlider_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void SetSliderActive(bool flag);
	void SetSliderTipPos(int32_t x);

private:
	bool m_sliderActive;
	int32_t m_sliderTipXPos;
};

