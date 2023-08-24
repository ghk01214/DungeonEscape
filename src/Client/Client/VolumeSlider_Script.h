#pragma once

#include "Bar_Script.h"

class VolumeSlider_Script final : public Bar_Script
{
public:
	VolumeSlider_Script(bool active = true);
	virtual ~VolumeSlider_Script();

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

public:
	void ChangeMuteTexture(bool mute);

private:
	void ChangeSliderTransform();
};

