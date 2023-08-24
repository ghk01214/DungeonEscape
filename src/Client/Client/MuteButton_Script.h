#pragma once

#include "Button_Script.h"

class MuteButton_Script final : public Button_Script
{
public:
	enum class SOUND_TYPE
	{
		NONE = 0,

		BGM,
		SE,

		MAX
	};
public:
	MuteButton_Script(SOUND_TYPE type);
	virtual ~MuteButton_Script();

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

private:
	TEXTURE_TYPE Mute();

public:
	constexpr bool IsMute() const { return m_mute; }

private:
	SOUND_TYPE m_soundType;

	bool m_mute;
	bool m_prevMute;
};

