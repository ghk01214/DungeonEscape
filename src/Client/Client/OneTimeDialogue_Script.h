﻿#pragma once

#include "UI_Script.h"

class OneTimeDialogue_Script : public UI_Script
{
public:
	enum class DIALOGUE_TYPE
	{
		NONE = 0,

		PILLAR_HINT,
		PILLAR_HINT2,
		ARTIFACT_DESTROYED,
		WEEPER_HINT,
		WEEPER_CAST4_HINT,
		GOLEM_HINT,

		MAX
	};
private:
	enum class FADE_TYPE
	{
		NONE = 0,

		FADE_IN,
		FADE_OUT,

		MAX,
	};
public:
	OneTimeDialogue_Script(const std::string& type);
	virtual ~OneTimeDialogue_Script();

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

public:
	void StartRender(float fadeDuration, float renderDuration);

public:
	void SetTimeData(float fadeDuration, float renderDuration);

	constexpr bool IsRendering() const { return m_render; }

private:
	bool m_render;
	float m_accTime;
	float m_fadeDuration;
	float m_renderDuration;

	FADE_TYPE m_fadeType;
	DIALOGUE_TYPE m_dialogueType;
};

