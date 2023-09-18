#pragma once

#include "UI_Script.h"

class ProgressImage_Script : public UI_Script
{
public:
	enum ICON_TYPE
	{
		PHASE1,
		PHASE2,
		PHASE3,
		PHASE4,
		PHASE5,
		PHASE6,
		PHASE7,

		MAX
	};
public:
	ProgressImage_Script();
	virtual ~ProgressImage_Script();

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

public:
	void ChangeImage(ICON_TYPE index);
	void ChangeImage(int32_t index);

public:
	void FadeIn(float time);
	void FadeOut(float time);

	void SetVisible(bool flag);

	constexpr bool IsFadingIn() const { return m_fadeIn; }

private:
	Vec3 m_pos;

	bool m_fadeIn;
	bool m_fadeOut;

	float m_fadeTime;
};

