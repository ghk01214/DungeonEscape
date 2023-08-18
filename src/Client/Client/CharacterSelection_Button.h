#pragma once

#include "Button_Script.h"

enum class BUTTON_TYPE
{
	NONE = 0,

	CHARACTER_SELECT,
	CHARACTER_NAME,
	GAME_READY,

	MAX
};

class CharacterSelection_Button : public Button_Script
{
public:
	CharacterSelection_Button(BUTTON_TYPE type);
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
	BUTTON_TYPE m_type;
	float m_accTime;
	int32_t m_blinkCount;
};

