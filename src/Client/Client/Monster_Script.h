#pragma once

#include "MonoBehaviour.h"

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
	void Transform(network::CPacket& packet);

protected:
	float m_radius;
	float m_halfHeight;

	bool m_aniEnd;
};