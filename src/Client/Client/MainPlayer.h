#pragma once

#include "GameObject.h"
#include "Transform.h"

class CMainPlayer : public CGameObject
{
public:
	CMainPlayer();
	~CMainPlayer();

public:
	void Update();
	void LateUpdate();

public:
	static std::vector<std::shared_ptr<CGameObject>> CreateMainPlayer(std::wstring _name, std::wstring fbxFilePath);
};

