#pragma once

#include "UI_Script.h"

class BossWarning_Script : public UI_Script
{
private:
	enum class FADE_TYPE
	{
		NONE = 0,

		FADE_IN,
		FADE_OUT,

		MAX,
	};
public:
	BossWarning_Script();
	virtual ~BossWarning_Script();

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

public:
	void StartBlink(float fadeDuration, float renderDuration, float blinkSpeed, float maxAlpha = 1.f);

public:
	void SetData(float fadeDuration, float renderDuration, float blinkSpeed, float maxAlpha = 1.f);
	void SetTimeData(float fadeDuration, float renderDuration);
	void SetSpeedDate(float blinkSpeed, float maxAlpha = 1.f);

private:
	bool m_render;
	float m_accTime;
	float m_fadeDuration;
	float m_renderDuration;

	int32_t m_blinkCount;
	float m_blinkSpeed;
	float m_maxAlpha;

	FADE_TYPE m_fadeType;
};

