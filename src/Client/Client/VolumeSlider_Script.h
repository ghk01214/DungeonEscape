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
	void ChangeSliderTransform();

public:
	void SetSliderActive(bool flag);

private:
	bool m_sliderActive;
};

