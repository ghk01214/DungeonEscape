#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "SceneManager.h"
#include <NetworkManager.h>

#include "GameInstance.h"
#include "Scene_Test.h"
#include "Player_Mistic.h"

void CGame::Init(const WindowInfo& Info, const std::wstring& ID, const std::wstring& PWD)
{
	GGameInstance->Initialize_Engine(Info);

	int32_t classNum{ 1 };
	//std::wcout << std::format(L"Warrior = 1\n");
	//std::wcout << std::format(L"Magician = 2\n");
	//std::wcout << std::format(L"Priest = 3\n");
	//std::wcout << std::format(L"Choose player class : \n");
	//std::wcin >> classNum;

	GET_SINGLE(SceneManager)->LoadScene(Scene_Test::GetInstance()->TestScene(magic_enum::enum_value<server::FBX_TYPE>(classNum)));

	GET_NETWORK->SendLoginPacket(ID, PWD);
}

void CGame::Start()
{
	GET_NETWORK->SendIDIssueRequest();
	GET_SINGLE(SceneManager)->Start();
}

void CGame::Update()
{
	GEngine->Update();

	Scene_Test::GetInstance()->Update();
}

void CGame::LateUpdate()
{
	Scene_Test::GetInstance()->LateUpdate();
}
