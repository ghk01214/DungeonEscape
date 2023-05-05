﻿#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "SceneManager.h"
#include <NetworkManager.h>

#include "GameInstance.h"
#include "Scene_Test.h"
#include "Player_Script.h"

void CGame::Init(const WindowInfo& Info)
{
	GGameInstance->Initialize_Engine(Info);

	GET_SINGLE(SceneManager)->LoadScene(Scene_Test::GetInstance()->TestScene());

	GET_NETWORK->AddScript(server::SCRIPT_TYPE::MISTIC, std::make_shared<Player_Mistic>());
	GET_NETWORK->SendLoginPacket();
}

void CGame::Update()
{
	GEngine->Update();
}
