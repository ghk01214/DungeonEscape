#include "pch.h"
#include "MainPlayer.h"
#include "MeshData.h"
#include "Resources.h"
#include "MeshRenderer.h"

CMainPlayer::CMainPlayer()
{
}

CMainPlayer::~CMainPlayer()
{
}

void CMainPlayer::Update()
{

}

void CMainPlayer::LateUpdate()
{

}

std::vector<std::shared_ptr<CGameObject>> CMainPlayer::CreateMainPlayer(std::wstring _name, std::wstring _fbxFilePath)
{
	shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(_fbxFilePath);

	if (nullptr == meshData)
	{
		std::wstring failedStr{ L"Failed to MainPlayer::CreateMainPlayer - " };
		failedStr += _name;
		MSG_BOX(failedStr.c_str());
	}

	vector<shared_ptr<CGameObject>> gameObjects = meshData->Instantiate_Player();

	for (auto& gameObject : gameObjects)
	{
		gameObject->SetName(_name);
		gameObject->SetCheckFrustum(false);
		std::shared_ptr<Material> mat = gameObject->GetMeshRenderer()->GetMaterial();
		gameObject->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
	}

	return gameObjects;
}
