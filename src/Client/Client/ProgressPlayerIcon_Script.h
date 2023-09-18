#pragma once

#include "UI_Script.h"

class ProgressPlayerIcon_Script : public UI_Script
{
public:
	enum PHASE_POS
	{
		PHASE1,
		PHASE2,
		PHASE3,
		PHASE4,
		PHASE5,
		PHASE6,

		MAX
	};
public:
	ProgressPlayerIcon_Script();
	virtual ~ProgressPlayerIcon_Script();

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

public:
	void SetOriginPos(Vec3 pos);
	void SetCurrentPos(Vec3 pos);
	void SetVisible(bool flag);
	void SetCurrentPhase(int32_t index);

	Vec3 GetOriginPos(int32_t index) { return m_originPos[index]; }

	void AddOriginPos(Vec3 pos);

	void FadeIn(float time);
	void FadeOut(float time);

	void ChangeCurrentPos(int32_t index);

private:
	std::vector<Vec3> m_originPos;
	Vec3 m_currPos;

	float m_fadeTime;

	bool m_fadeIn;
	bool m_fadeOut;

	int32_t m_sign;
	int32_t m_currPhase;
};

