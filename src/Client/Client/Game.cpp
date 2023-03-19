#include "pch.h"
#include "Network.h"
#include "Game.h"
#include "Engine.h"
#include "SceneManager.h"

#include "GameInstance.h"
#include "Scene_Test.h"

void CGame::Init(const WindowInfo& Info)
{
	GGameInstance->Initialize_Engine(Info);

	GET_SINGLE(SceneManager)->LoadScene(Scene_Test::GetInstance()->TestScene());

	network::pNetwork->SendLoginPacket();
}

void CGame::Update()
{
	GEngine->Update();
}
