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
	void ChangeTransform(Vec3& scale);
	void ChangeAlpha();

protected:
	int32_t m_repeatCount;	// 최대반복횟수
	int32_t m_curCount;		// 현재 반복되고 있는 횟수

	float m_duration;		// 해당 오브젝트 지속시간
	float m_accTime;		// 시간을 축척하기 위한 변수
	float m_speed;		// 축적에 걸리는 시간의 속도

	float m_alpha;		// Alpha값

	Vec3 m_scale;		// 구체 스케일
};