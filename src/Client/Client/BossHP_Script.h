#pragma once
#include "Bar_Script.h"

class BossHP_Script final : public Bar_Script
{
public:
	BossHP_Script(int32_t hp = 100);
	virtual ~BossHP_Script();

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

private:
	void ChangeHPTransform();

public:
	constexpr int32_t GetHP() const { return m_currHP; }

	void SetHP(int32_t hp);
	void SetByDamage(int32_t damage);

private:
	int32_t m_currHP;
	int32_t m_prevHP;

	float m_hpRatio;
	float m_barLengthRatio;

	bool m_hpChanged;
};

