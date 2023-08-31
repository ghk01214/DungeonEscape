#pragma once

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
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void StartRender(float fadeDuration, float renderDuration);

public:
	void SetTimeData(float fadeDuration, float renderDuration);

private:
	bool m_render;
	float m_accTime;
	float m_fadeDuration;
	float m_renderDuration;

	FADE_TYPE m_fadeType;
	DIALOGUE_TYPE m_dialogueType;
};

