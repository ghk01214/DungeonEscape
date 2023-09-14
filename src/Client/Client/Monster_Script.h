#pragma once

#include "MonoBehaviour.h"

class BossCounterEffect_Script;

class Monster_Script : public MonoBehaviour
{
public:
	Monster_Script();
	~Monster_Script();

public:
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

protected:
	virtual void CheckState() = 0;
	virtual void UpdateFrameRepeat() = 0;
	virtual void UpdateFrameOnce() = 0;

protected:
	virtual void Transform(network::CPacket& packet);
	void Rotate(network::CPacket& packet);
	void CounterEffect(network::CPacket& packet);

public:
	void SetCounterEffectScript(std::shared_ptr<BossCounterEffect_Script> script);

protected:
	float m_radius;
	float m_halfHeight;

	bool m_aniEnd;

	std::shared_ptr<BossCounterEffect_Script> m_counterEffectScript;
};