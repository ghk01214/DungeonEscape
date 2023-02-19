#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "SceneManager.h"

void CGame::Init(const WindowInfo& Info, std::shared_ptr<network::CNetwork> pNetwork)
{
	g_Engine->Init(Info, pNetwork);

	GET_SINGLE(CSceneManager)->LoadScene(L"TestScene");
}

void CGame::Update()
{
	g_Engine->Update();
}
