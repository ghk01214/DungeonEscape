#pragma once

#include "MonoBehaviour.h"

class PlayerRangeAttack : public MonoBehaviour
{
public:
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

private:
	void ParsePackets();
	void Transform(network::CPacket& packet);

};

