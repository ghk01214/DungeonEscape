#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

class UI abstract : public CGameObject
{
public:
	explicit UI();
	virtual ~UI() = default;

public:
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FinalUpdate();
	virtual void Render();

private:
	std::wstring m_texture;
};

