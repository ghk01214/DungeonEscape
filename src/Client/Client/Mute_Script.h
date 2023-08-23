#pragma once

#include "Button_Script.h"

class Mute_Script : public Button_Script
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
	Mute_Script(SOUND_TYPE type);
	virtual ~Mute_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

private:
	TEXTURE_TYPE Mute();

public:
	constexpr bool IsMute() const { return m_mute; }

private:
	SOUND_TYPE m_soundType;

	bool m_mute;
	bool m_prevMute;
};

