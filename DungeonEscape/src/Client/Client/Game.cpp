#include "pch.h"
#include "Game.h"
#include "Engine.h"

void CGame::Init(const WindowInfo& Info)
{
	g_Engine->Init(Info);
}

void CGame::Update()
{
	g_Engine->Render();
}
