﻿#pragma once

#include "Client_Defines.h"

class CGame
{
public:
	void Init(const WindowInfo& Info);
	void Start();
	void Update();
	void Release(void);

private:
	void Open_Scene(SCENE eSceneIndex);
};

