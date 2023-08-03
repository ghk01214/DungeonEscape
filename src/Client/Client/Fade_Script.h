﻿#pragma once

#include "MonoBehaviour.h"

class Texture;

class Fade_Script : public MonoBehaviour
{
public:
	enum class Fade_State
	{
		Fade_In,
		Contents,
		Fade_Out,
		END
	};

public:
	Fade_Script();
	virtual ~Fade_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void SetLogoInfo(float fadeInDuration, float contentsDuration, float fadeOutDuration, vector<shared_ptr<Texture>> textures);

	bool GetLogoEnd(void) { return m_bLogoEnd; }

	void SetActivate(bool bActivate) { m_bActivate = bActivate; }

protected:
	float m_fAlpha = 1.f;		// Alpha값

	float m_fadeInDuration = 1.5f;	// Fade In에 걸리는 시간
	float m_fadeOutDuration = 1.5f;	// Fade Out에 걸리는 시간
	float m_contentsDuration = 5.f;	// 내용을 보여주는데 걸리는 시간
	float m_accTime = 0.f;		// 시간을 축척하기 위한 변수

	bool m_bLogoEnd = false;	// 해당 스크립트가 끝났는지 확인하는 변수. True면 끝났다는 의미

	Fade_State m_eState = Fade_State::END;

	uint32 m_curTextureCount = 0;
	vector<shared_ptr<Texture>> m_textures;

	bool m_bActivate = false;
};