#include "pch.h"
#include "../Engine/Network.h"
#include "Game.h"
#include "Engine.h"
#include "SceneManager.h"

void CGame::Init(const WindowInfo& Info)
{
	g_Engine->Init(Info);

	GET_SINGLE(CSceneManager)->LoadScene(L"TestScene");
}

void CGame::Update()
{
	g_Engine->Update();
}
