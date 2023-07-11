#pragma once

#include "Client_Defines.h"

class CGame
{
public:
	void Init(const WindowInfo& Info, const std::wstring& ID, const std::wstring& PWD);
	void Start();
	void Update();

private:
	void Open_Scene(SCENE eSceneIndex);
};

