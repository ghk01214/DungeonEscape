#pragma once

#include "GameObject.h"

enum WEAPON_TYPE
{
	WT_WAND,
	WT_END
};

class CWeapon final : public CGameObject
{

public:
	explicit CWeapon();
	virtual ~CWeapon() = default;

public:
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FinalUpdate();
	virtual void Render();

public:
	void SetWeaponType(WEAPON_TYPE eWeapon) { m_eWeapon = eWeapon; }

private:
	WEAPON_TYPE m_eWeapon;
};

