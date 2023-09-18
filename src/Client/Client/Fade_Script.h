#pragma once

#include "MonoBehaviour.h"
#include "Scene_Test.h"

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
	void SetLogoInfo(float fadeInDuration, float fadeOutDuration, vector<shared_ptr<Texture>> textures);
	void SetDurationTime(float fadeInDuration, float fadeOutDuration);

	bool GetLogoEnd(void) { return m_bLogoEnd; }
	constexpr bool GetActivation() const { return m_bActivate; }

	void SetActivate(bool bActivate) { m_bActivate = bActivate; }

	void FadeIn(void) { m_bFadeIn = true; m_bFadeOut = false; }
	void FadeOut(void) { m_bFadeOut = true; }
	bool GetFading(void) { return m_bFading; }
	void SetMapType(MAP_TYPE eType) { m_eMoveMapType = eType; }
	MAP_TYPE GetMapType(void) { return m_eMoveMapType; }

	void SetFade(std::shared_ptr<bool> in, std::shared_ptr<bool> out);

protected:
	float m_fAlpha = 1.f;		// Alpha값

	float m_fadeInDuration = 1.5f;	// Fade In에 걸리는 시간
	float m_fadeOutDuration = 1.5f;	// Fade Out에 걸리는 시간
	//float m_contentsDuration = 5.f;	// 내용을 보여주는데 걸리는 시간
	float m_accTime = 0.f;		// 시간을 축척하기 위한 변수

	bool m_bLogoEnd = false;	// 해당 스크립트가 끝났는지 확인하는 변수. True면 끝났다는 의미

	Fade_State m_eState = Fade_State::END;

	uint32 m_curTextureCount = 0;
	vector<shared_ptr<Texture>> m_textures;

	bool m_bActivate = false;

private:
	bool m_bFadeIn = false;
	bool m_bFadeOut = false;

private:
	bool m_bFading = false;
	MAP_TYPE m_eMoveMapType = MAP_TYPE::END;

private:
	std::shared_ptr<bool> m_fadeIn;
	std::shared_ptr<bool> m_fadeOut;
};