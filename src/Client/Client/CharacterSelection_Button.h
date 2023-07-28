#pragma once

#include "Button_Script.h"

class CharacterSelection_Button : public Button_Script
{
public:
	CharacterSelection_Button();
	virtual ~CharacterSelection_Button();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void BlinkCharacter();
	void FadeOutToShowLoadingScene();

public:
	constexpr int32_t GetBlinkCount() const { return m_blinkCount; }

	void SetBlinkCount(int32_t count);

private:
	float m_accTime;
	int32_t m_blinkCount;
};

