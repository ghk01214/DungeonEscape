#pragma once

#include "GameObject.h"

#include "SceneManager.h"

class CGameInstance
{
public:
	void Initialize_Engine(const WindowInfo& info);

public:
	// scene 생성 관련 함수
	void LoadScene(std::shared_ptr<CScene> scene);
	void SetLayerName(uint8 index, const wstring& name);
	const wstring& IndexToLayerName(uint8 index);
	uint8 LayerNameToIndex(const wstring& name);
	shared_ptr<class GameObject> Pick(int32 screenX, int32 screenY);
	shared_ptr<CScene> GetActiveScene();


private:
	SceneManager* m_sceneManager = nullptr;
};
