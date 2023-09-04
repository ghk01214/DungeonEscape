#pragma once

#include "MonoBehaviour.h"

class MonsterRangeAttack : public MonoBehaviour
{
public:
	explicit MonsterRangeAttack(server::FBX_TYPE attackType, uint32_t index = 0);
	virtual ~MonsterRangeAttack();

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

protected:
	void ParsePackets();
	void Transform(network::CPacket& packet);

	void RenderEffect();

private:
	server::FBX_TYPE m_attackType;
	uint32_t m_effectIndex;
};

