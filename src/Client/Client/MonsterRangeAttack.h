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

public:
	void SetRenderFlag(bool flag);

public:
	constexpr uint32_t GetEffectIndex() const { return m_effectIndex; }

private:
	server::FBX_TYPE m_attackType;
	uint32_t m_effectIndex;

	bool m_render;
};

