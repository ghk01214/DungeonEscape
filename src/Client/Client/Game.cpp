#include "pch.h"
#include "Network.h"
#include "Game.h"
#include "Engine.h"
#include "SceneManager.h"

#include "GameInstance.h"
#include "Scene_Test.h"

void CGame::Init(const WindowInfo& Info, std::shared_ptr<network::CNetwork> pNetwork)
{
	GGameInstance->Initialize_Engine(Info, pNetwork);

	GET_SINGLE(SceneManager)->LoadScene(Scene_Test::GetInstance()->TestScene());
}

void CGame::Update()
{
	GEngine->Update();
}
