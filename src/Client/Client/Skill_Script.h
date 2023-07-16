#pragma once

#include "MonoBehaviour.h"

class Skill_Script : public MonoBehaviour
{
public:
	Skill_Script();
	virtual ~Skill_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

protected:
	uint32 m_repeatCount = 1;	// 최대반복횟수
	uint32 m_curCount = 0;		// 현재 반복되고 있는 횟수

	float m_duration = 1.f;		// 해당 오브젝트 지속시간
	float m_accTime = 0.f;		// 시간을 축척하기 위한 변수
	float m_speed = 1.f;		// 축적에 걸리는 시간의 속도

	float m_fAlpha = 1.f;		// Alpha값
};