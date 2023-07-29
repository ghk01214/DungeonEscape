#pragma once

#include "MonoBehaviour.h"

class MonsterRangeAttack : public MonoBehaviour
{
public:
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

protected:
	void Transform(network::CPacket& packet);

};

